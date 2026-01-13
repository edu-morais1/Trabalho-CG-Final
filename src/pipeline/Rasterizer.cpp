#include "Rasterizer.h"
#include "../core/Scene.h"
#include "../math/Matrix.h"
#include "../math/Vector.h"
#include "Shading.h"
#include "Transform.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

// ============ IMPLEMENTAÇÃO DO FRAMEBUFFER ============

Framebuffer::Framebuffer(int w, int h)
    : width(w), height(h), color(w * h, 0xff000000), depth(w * h, -1e9) {}

void Framebuffer::clear(uint32_t c) {
  std::fill(color.begin(), color.end(), c);
  std::fill(depth.begin(), depth.end(), -1e9);
}

void Framebuffer::putPixel(int x, int y, double z, const Vec3 &col) {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;
  int idx = y * width + x;
  if (z > depth[idx]) {
    depth[idx] = z;
    // Converte RGB [0,1] para ARGB uint32
    uint8_t r = static_cast<uint8_t>(std::clamp(col.x, 0.0, 1.0) * 255.0);
    uint8_t g = static_cast<uint8_t>(std::clamp(col.y, 0.0, 1.0) * 255.0);
    uint8_t b = static_cast<uint8_t>(std::clamp(col.z, 0.0, 1.0) * 255.0);
    color[idx] = 0xff000000 | (r << 16) | (g << 8) | b;
  }
}

// ============ ESTRUTURAS AUXILIARES ============

struct Vertex {
  Vec3 screen; // coordenadas de tela (x, y, z)
  Vec3 world;  // posição em espaço mundo
  Vec3 normal; // normal do vértice
};

struct Barycentric {
  double u, v, w;
  bool inside() const { return u >= 0 && v >= 0 && w >= 0; }
};

// ============ FUNÇÕES AUXILIARES ============

Barycentric barycentric(double px, double py, const Vertex &a, const Vertex &b,
                        const Vertex &c) {
  double x0 = a.screen.x, y0 = a.screen.y;
  double x1 = b.screen.x, y1 = b.screen.y;
  double x2 = c.screen.x, y2 = c.screen.y;

  double denom = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
  if (std::abs(denom) < 1e-10) {
    return {-1, -1, -1}; // triângulo degenerado
  }

  double u = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) / denom;
  double v = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / denom;
  double w = 1.0 - u - v;

  return {u, v, w};
}

// ============ RASTERIZAÇÃO DE TRIÂNGULOS ============

void rasterizeTriangle(Framebuffer &fb, const Vertex &v0, const Vertex &v1,
                       const Vertex &v2, const Vec3 &faceNormal,
                       const Material &mat, const std::vector<Light> &lights,
                       const Vec3 &eyePos, bool usePhong) {
  // Bounding box do triângulo
  int minX = std::max(
      0, (int)std::floor(std::min({v0.screen.x, v1.screen.x, v2.screen.x})));
  int maxX = std::min(
      fb.width - 1,
      (int)std::ceil(std::max({v0.screen.x, v1.screen.x, v2.screen.x})));
  int minY = std::max(
      0, (int)std::floor(std::min({v0.screen.y, v1.screen.y, v2.screen.y})));
  int maxY = std::min(
      fb.height - 1,
      (int)std::ceil(std::max({v0.screen.y, v1.screen.y, v2.screen.y})));

  // Flat shading: calcula cor uma vez
  Vec3 faceCenter = (v0.world + v1.world + v2.world) * (1.0 / 3.0);
  Vec3 flatColor =
      computeLighting(faceNormal, faceCenter, mat, lights, eyePos, false);

  // Scanline: percorre pixels do bounding box
  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      Barycentric bc = barycentric(x + 0.5, y + 0.5, v0, v1, v2);

      if (!bc.inside())
        continue;

      // Interpolar profundidade
      double z = bc.u * v0.screen.z + bc.v * v1.screen.z + bc.w * v2.screen.z;

      Vec3 color;
      if (usePhong) {
        // Phong: interpola normal e posição
        Vec3 normal = (v0.normal * bc.u + v1.normal * bc.v + v2.normal * bc.w)
                          .normalized();
        Vec3 pos = v0.world * bc.u + v1.world * bc.v + v2.world * bc.w;
        color = computeLighting(normal, pos, mat, lights, eyePos, true);
      } else {
        // Flat: usa cor pré-calculada
        color = flatColor;
      }

      // Z-buffer test e write
      fb.putPixel(x, y, z, color);
    }
  }
}

// ============ DADOS DO CUBO ============

// Faces do cubo (12 triângulos = 6 faces × 2 triângulos)
static const std::array<std::array<int, 3>, 12> cubeFaces = {{
    {4, 5, 6},
    {4, 6, 7}, // Front (z = +0.5)
    {1, 0, 3},
    {1, 3, 2}, // Back (z = -0.5)
    {5, 1, 2},
    {5, 2, 6}, // Right (x = +0.5)
    {0, 4, 7},
    {0, 7, 3}, // Left (x = -0.5)
    {7, 6, 2},
    {7, 2, 3}, // Top (y = +0.5)
    {0, 1, 5},
    {0, 5, 4} // Bottom (y = -0.5)
}};

// Normais de cada face (sem transformação)
static const std::array<Vec3, 6> faceNormals = {{
    {0, 0, 1},  // front
    {0, 0, -1}, // back
    {1, 0, 0},  // right
    {-1, 0, 0}, // left
    {0, 1, 0},  // top
    {0, -1, 0}  // bottom
}};

// ============ RENDERIZAÇÃO DO CUBO ============

void renderCube(const Cube &cube, const Camera &camera,
                const std::vector<Light> &lights, Framebuffer &fb,
                bool usePhong) {
  // 1. Vértices do cubo no SRU (object space)
  Vec3 baseVerts[8];
  Cube::baseVertices(baseVerts);

  // 2. Matrizes de transformação
  Mat4 model = cube.modelMatrix();
  Mat4 view = camera.viewMatrix();
  Mat4 proj = camera.projectionMatrix();

  // 3. Transformar vértices para clip space e tela
  Vertex verts[8];
  for (int i = 0; i < 8; ++i) {
    // Para mundo (ORDEM CORRETA: model * ponto)
    Vec4 worldPos4 = model * toVec4(baseVerts[i]);
    verts[i].world = Vec3{worldPos4.x, worldPos4.y, worldPos4.z};

    // Para NDC
    Vec3 ndc = transformToNDC(baseVerts[i], model, view, proj);

    // NDC [-1,1] → tela [0, width/height]
    verts[i].screen.x = (ndc.x + 1.0) * 0.5 * fb.width;
    verts[i].screen.y = (1.0 - ndc.y) * 0.5 * fb.height; // Y invertido
    verts[i].screen.z = ndc.z;                           // depth para z-buffer
  }

  // 4. Rasterizar cada face (12 triângulos)
  for (size_t f = 0; f < 12; ++f) {
    int i0 = cubeFaces[f][0];
    int i1 = cubeFaces[f][1];
    int i2 = cubeFaces[f][2];

    // Normal da face transformada para world space (ORDEM CORRETA)
    int faceIdx = f / 2; // cada face tem 2 triângulos
    Vec3 normal = faceNormals[faceIdx];
    Vec4 normalWorld4 = model * toVec4(normal, 0.0); // w=0 para vetores
    Vec3 faceNormal =
        Vec3{normalWorld4.x, normalWorld4.y, normalWorld4.z}.normalized();

    // Back-face culling: se normal aponta para longe da câmera, pula
    Vec3 viewDir = (camera.eye - verts[i0].world).normalized();
    if (faceNormal.dot(viewDir) < 0)
      continue;

    // Normais dos vértices (para Phong shading)
    verts[i0].normal = faceNormal;
    verts[i1].normal = faceNormal;
    verts[i2].normal = faceNormal;

    // Rasterizar triângulo
    rasterizeTriangle(fb, verts[i0], verts[i1], verts[i2], faceNormal,
                      cube.material, lights, camera.eye, usePhong);
  }
}

// ============ RENDERIZAÇÃO DA CENA ============

void renderScene(const Scene &scene, Framebuffer &fb, bool usePhong) {
  // Renderizar cada cubo da cena
  for (const auto &cube : scene.cubes) {
    renderCube(cube, scene.camera, scene.lights, fb, usePhong);
  }
}
