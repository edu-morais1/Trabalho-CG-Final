#include "Rasterizer.h"
#include "../core/Scene.h"
#include "../math/Matrix.h"
#include "../math/Vector.h"
#include "Pipeline.h"
#include "Shading.h"
#include "Transform.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

// =============================================================================
//  FRAMEBUFFER
// =============================================================================

Framebuffer::Framebuffer(int w, int h)
    : width(w), height(h),
      color(w * h, 0xff000000),
      depth(w * h, std::numeric_limits<double>::infinity()) {}

void Framebuffer::clear(uint32_t c) {
    std::fill(color.begin(), color.end(), c);
    std::fill(depth.begin(), depth.end(),
              std::numeric_limits<double>::infinity());
}

void Framebuffer::putPixel(int x, int y, double z, const Vec3 &col) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    int idx = y * width + x;
    if (z <= depth[idx]) {   // menor z = mais perto
        depth[idx] = z;
        uint8_t r = (uint8_t)(std::clamp(col.x, 0.0, 1.0) * 255.0);
        uint8_t g = (uint8_t)(std::clamp(col.y, 0.0, 1.0) * 255.0);
        uint8_t b = (uint8_t)(std::clamp(col.z, 0.0, 1.0) * 255.0);
        color[idx] = 0xff000000u | (r << 16) | (g << 8) | b;
    }
}

// =============================================================================
//  CLIP VERTEX — posição em clip space (x,y,z,w) + normal interpolável
// =============================================================================

struct ClipVertex {
    double x, y, z, w;
    double nx, ny, nz;
};

// =============================================================================
//  CLIPPING SUTHERLAND-HODGMAN 3D (clip space homogêneo)
//  Equivalente ao recorteSH() de Cena.java
// =============================================================================

static bool insidePlane(const ClipVertex &v, const double plane[4]) {
    double d = v.x * plane[0] + v.y * plane[1]
             + v.z * plane[2] + v.w * plane[3];
    return d >= -1e-5;
}

static ClipVertex intersectPlane(const ClipVertex &a, const ClipVertex &b,
                                  const double plane[4]) {
    double da = a.x*plane[0] + a.y*plane[1] + a.z*plane[2] + a.w*plane[3];
    double db = b.x*plane[0] + b.y*plane[1] + b.z*plane[2] + b.w*plane[3];
    double denom = da - db;
    if (std::abs(denom) < 1e-9) return b;
    double t = da / denom;
    ClipVertex r;
    r.x  = a.x  + t * (b.x  - a.x);
    r.y  = a.y  + t * (b.y  - a.y);
    r.z  = a.z  + t * (b.z  - a.z);
    r.w  = a.w  + t * (b.w  - a.w);
    r.nx = a.nx + t * (b.nx - a.nx);
    r.ny = a.ny + t * (b.ny - a.ny);
    r.nz = a.nz + t * (b.nz - a.nz);
    return r;
}

static std::vector<ClipVertex>
clipPolygonSH(const std::vector<ClipVertex> &poly) {
    // 6 planos em clip space homogêneo — iguais ao Cena.java
    static const double planes[6][4] = {
        { 0,  0,  1, 1},  // near:   z + w >= 0
        { 0,  0, -1, 1},  // far:   -z + w >= 0
        { 1,  0,  0, 1},  // left:   x + w >= 0
        {-1,  0,  0, 1},  // right: -x + w >= 0
        { 0,  1,  0, 1},  // bottom: y + w >= 0
        { 0, -1,  0, 1}   // top:   -y + w >= 0
    };

    std::vector<ClipVertex> output = poly;

    for (int p = 0; p < 6; ++p) {
        if (output.empty()) return output;
        std::vector<ClipVertex> input = output;
        output.clear();
        int n = (int)input.size();

        for (int i = 0; i < n; ++i) {
            const ClipVertex &curr = input[i];
            const ClipVertex &prev = input[(i - 1 + n) % n];
            bool currIn = insidePlane(curr, planes[p]);
            bool prevIn = insidePlane(prev, planes[p]);

            if (currIn) {
                if (!prevIn)
                    output.push_back(intersectPlane(prev, curr, planes[p]));
                output.push_back(curr);
            } else if (prevIn) {
                output.push_back(intersectPlane(prev, curr, planes[p]));
            }
        }
    }
    return output;
}

// =============================================================================
//  VÉRTICE DE TELA + ARESTA (Edge Table / Active Edge Table)
//  Equivalente a VerticeTela e Aresta de Cena.java
// =============================================================================

struct VertexScreen {
    double x, y, z;
    Vec3   normal;
};

struct Edge {
    int    yMax;
    double x,    dx_dy;
    double z,    dz_dy;
    double nx,   dnx_dy;
    double ny,   dny_dy;
    double nz,   dnz_dy;
};

// =============================================================================
//  RASTERIZAÇÃO SCANLINE ET/AET
//  Equivalente ao rasterizarFace() de Cena.java
// =============================================================================

static void rasterizeFace(
    const std::vector<VertexScreen> &verts,
    bool                             usePhong,
    const Vec3                      &flatColor,
    const Vec3                      &faceWorldCenter, // posição para lighting Phong
    const Material                  &mat,
    const std::vector<Light>        &lights,
    const Vec3                      &eyePos,
    Framebuffer                     &fb)
{
    int n = (int)verts.size();
    if (n < 3) return;

    // ── Limites em Y ──────────────────────────────────────────────────────────
    double minY_d = verts[0].y, maxY_d = verts[0].y;
    for (const auto &v : verts) {
        minY_d = std::min(minY_d, v.y);
        maxY_d = std::max(maxY_d, v.y);
    }
    int y_min = std::max(0,              (int)minY_d);
    int y_max = std::min(fb.height - 1,  (int)maxY_d);
    if (y_min > y_max) return;

    // ── Edge Table ET[y - y_min] ───────────────────────────────────────────────
    int rows = y_max - y_min + 1;
    std::vector<std::vector<Edge>> ET(rows);

    for (int i = 0; i < n; ++i) {
        VertexScreen p1 = verts[i];
        VertexScreen p2 = verts[(i + 1) % n];
        if (p1.y > p2.y) std::swap(p1, p2);

        double dy = p2.y - p1.y;
        if (dy < 1e-6) continue;   // aresta horizontal — ignorar

        double dx_dy = (p2.x - p1.x) / dy;
        double dz_dy = (p2.z - p1.z) / dy;

        int    start_y = (int)std::ceil(p1.y);
        double offset  = start_y - p1.y;

        Edge e;
        e.yMax   = (int)std::ceil(p2.y);
        e.x      = p1.x + offset * dx_dy;
        e.dx_dy  = dx_dy;
        e.z      = p1.z + offset * dz_dy;
        e.dz_dy  = dz_dy;

        if (usePhong) {
            e.dnx_dy = (p2.normal.x - p1.normal.x) / dy;
            e.dny_dy = (p2.normal.y - p1.normal.y) / dy;
            e.dnz_dy = (p2.normal.z - p1.normal.z) / dy;
            e.nx = p1.normal.x + offset * e.dnx_dy;
            e.ny = p1.normal.y + offset * e.dny_dy;
            e.nz = p1.normal.z + offset * e.dnz_dy;
        } else {
            e.nx = e.ny = e.nz = 0.0;
            e.dnx_dy = e.dny_dy = e.dnz_dy = 0.0;
        }

        int row = start_y - y_min;
        if (row >= 0 && row < rows)
            ET[row].push_back(e);
    }

    // ── Scan: percorre y, atualiza AET, preenche spans ────────────────────────
    std::vector<Edge> AET;

    for (int y = y_min; y <= y_max; ++y) {
        // Inserir novas arestas
        for (auto &e : ET[y - y_min])
            AET.push_back(e);

        // Remover arestas vencidas
        AET.erase(
            std::remove_if(AET.begin(), AET.end(),
                           [y](const Edge &e) { return y >= e.yMax; }),
            AET.end());

        // Ordenar por x crescente
        std::sort(AET.begin(), AET.end(),
                  [](const Edge &a, const Edge &b) { return a.x < b.x; });

        // Preencher spans (par de arestas)
        for (int i = 0; i + 1 < (int)AET.size(); i += 2) {
            Edge &e1 = AET[i];
            Edge &e2 = AET[i + 1];

            int x_start = std::max(0,             (int)std::ceil(e1.x));
            int x_end   = std::min(fb.width - 1,  (int)std::ceil(e2.x));
            if (x_end <= x_start) continue;

            double span = e2.x - e1.x;
            if (span < 1e-6) span = 1.0;

            double dz_dx  = (e2.z - e1.z) / span;
            double offset_x = x_start - e1.x;
            double z = e1.z + offset_x * dz_dx;

            double nx = 0, ny = 0, nz = 0;
            double dnx_dx = 0, dny_dx = 0, dnz_dx = 0;
            if (usePhong) {
                dnx_dx = (e2.nx - e1.nx) / span;
                dny_dx = (e2.ny - e1.ny) / span;
                dnz_dx = (e2.nz - e1.nz) / span;
                nx = e1.nx + offset_x * dnx_dx;
                ny = e1.ny + offset_x * dny_dx;
                nz = e1.nz + offset_x * dnz_dx;
            }

            for (int x = x_start; x < x_end; ++x) {
                int idx = y * fb.width + x;
                if (z <= fb.depth[idx]) {
                    fb.depth[idx] = z;

                    Vec3 color = flatColor;
                    if (usePhong) {
                        Vec3 N = Vec3{nx, ny, nz}.normalized();
                        // faceWorldCenter como posição aproximada do fragmento
                        color = computeLighting(N, faceWorldCenter, mat,
                                                lights, eyePos, true);
                    }

                    uint8_t cr = (uint8_t)(std::clamp(color.x, 0.0, 1.0) * 255.0);
                    uint8_t cg = (uint8_t)(std::clamp(color.y, 0.0, 1.0) * 255.0);
                    uint8_t cb = (uint8_t)(std::clamp(color.z, 0.0, 1.0) * 255.0);
                    fb.color[idx] = 0xff000000u | (cr << 16) | (cg << 8) | cb;
                }

                z += dz_dx;
                if (usePhong) { nx += dnx_dx; ny += dny_dx; nz += dnz_dx; }
            }
        }

        // Avançar arestas para próxima scanline
        for (auto &e : AET) {
            e.x  += e.dx_dy;
            e.z  += e.dz_dy;
            if (usePhong) {
                e.nx += e.dnx_dy;
                e.ny += e.dny_dy;
                e.nz += e.dnz_dy;
            }
        }
    }
}

// =============================================================================
//  DADOS DO CUBO
// =============================================================================

static const std::array<std::array<int, 3>, 12> cubeFaces = {{
    {4, 5, 6}, {4, 6, 7},  // Front  (z = +0.5)
    {1, 0, 3}, {1, 3, 2},  // Back   (z = -0.5)
    {5, 1, 2}, {5, 2, 6},  // Right  (x = +0.5)
    {0, 4, 7}, {0, 7, 3},  // Left   (x = -0.5)
    {7, 6, 2}, {7, 2, 3},  // Top    (y = +0.5)
    {0, 1, 5}, {0, 5, 4}   // Bottom (y = -0.5)
}};

static const std::array<Vec3, 6> faceNormals = {{
    { 0,  0,  1},  // front
    { 0,  0, -1},  // back
    { 1,  0,  0},  // right
    {-1,  0,  0},  // left
    { 0,  1,  0},  // top
    { 0, -1,  0}   // bottom
}};

// =============================================================================
//  RENDER CUBE — pipeline completo A..M + culling + clipping + scanline
// =============================================================================

static ClipVertex makeClipVertex(const Vec3 &worldPos, const Vec3 &normal,
                                  const Mat4 &m_total) {
    Vec4 clip = m_total * toVec4(worldPos);
    ClipVertex cv;
    cv.x = clip.x; cv.y = clip.y; cv.z = clip.z; cv.w = clip.w;
    cv.nx = normal.x; cv.ny = normal.y; cv.nz = normal.z;
    return cv;
}

static void renderCube(const Cube &cube, const Camera &camera,
                       const std::vector<Light> &lights,
                       Framebuffer &fb, bool usePhong) {
    Vec3 baseVerts[8];
    Cube::baseVertices(baseVerts);
    Mat4 model = cube.modelMatrix();

    // ── Pipeline A..M ─────────────────────────────────────────────────────────
    Mat4 m_view  = Pipeline::A(camera.vrp)
                * Pipeline::B(camera.u, camera.v, camera.n);   // A primeiro (translada), B segundo (rotaciona)

    Mat4 m_proj  = Pipeline::C(camera.Cu, camera.Cv, camera.DP)
                * Pipeline::D(camera.Su, camera.Sv, camera.DP, camera.farPlane)
                * Pipeline::P(camera.farPlane, camera.nearPlane); // C→D→P

    Mat4 m_total = m_view * m_proj;                              // view primeiro, proj segundo

    Mat4 m_screen = Pipeline::J()
                  * Pipeline::K()
                  * Pipeline::L(fb.width,  0,
                                fb.height, 0,
                                camera.zMax, camera.zMin)
                  * Pipeline::M();        // NDC → pixels

    // ── Vértices em espaço de mundo ───────────────────────────────────────────
    Vec3 worldVerts[8];
    for (int i = 0; i < 8; ++i) {
        Vec4 w4 = model * toVec4(baseVerts[i]);
        worldVerts[i] = Vec3{w4.x, w4.y, w4.z};
    }

    // ── Loop por face/triângulo ───────────────────────────────────────────────
    for (size_t f = 0; f < 12; ++f) {
        int i0 = cubeFaces[f][0];
        int i1 = cubeFaces[f][1];
        int i2 = cubeFaces[f][2];

        // Normal da face transformada para mundo
        Vec4 n4 = model * toVec4(faceNormals[f / 2], 0.0);
        Vec3 faceNormal = Vec3{n4.x, n4.y, n4.z}.normalized();

        // ── Back-face culling (padrão Rafael/Alvy) ────────────────────────────
        Vec3 toCamera = camera.eye - worldVerts[i0];
        if (faceNormal.dot(toCamera) <= 0.0) continue;

        // ── Montar triângulo em clip space ────────────────────────────────────
        std::vector<ClipVertex> clipPoly = {
            makeClipVertex(worldVerts[i0], faceNormal, m_total),
            makeClipVertex(worldVerts[i1], faceNormal, m_total),
            makeClipVertex(worldVerts[i2], faceNormal, m_total)
        };

        // ── Clipping Sutherland-Hodgman ───────────────────────────────────────
        clipPoly = clipPolygonSH(clipPoly);
        if (clipPoly.size() < 3) continue;

        // ── Divisão por w + m_screen → coordenadas de tela ───────────────────
        std::vector<VertexScreen> screenVerts;
        screenVerts.reserve(clipPoly.size());
        for (const auto &cv : clipPoly) {
            double w = std::abs(cv.w) > 1e-6 ? cv.w : 1e-6;
            Vec4 ndc = {cv.x / w, cv.y / w, cv.z / w, 1.0};
            Vec4 sc  = m_screen * ndc;
            VertexScreen vs;
            vs.x = sc.x; vs.y = sc.y; vs.z = sc.z;
            vs.normal = Vec3{cv.nx, cv.ny, cv.nz};
            screenVerts.push_back(vs);
        }

        // ── Cor flat (calculada uma vez por face) ─────────────────────────────
        Vec3 flatColor = cube.material.color;
        if (!lights.empty())
            flatColor = computeLighting(faceNormal, worldVerts[i0],
                                         cube.material, lights, camera.eye, false);

        // Centro da face em mundo (posição aproximada para Phong por pixel)
        Vec3 faceCenter = (worldVerts[i0] + worldVerts[i1] + worldVerts[i2])
                        * (1.0 / 3.0);

        // ── Rasterização scanline ─────────────────────────────────────────────
        rasterizeFace(screenVerts, usePhong, flatColor, faceCenter,
                      cube.material, lights, camera.eye, fb);
    }
}

// =============================================================================
//  RENDER SCENE
// =============================================================================

void renderScene(const Scene &scene, Framebuffer &fb, bool usePhong) {
    for (const auto &cube : scene.cubes)
        renderCube(cube, scene.camera, scene.lights, fb, usePhong);
}

