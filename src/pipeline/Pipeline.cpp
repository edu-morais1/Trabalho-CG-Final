#include "Pipeline.h"


// { 1, 0, 0, -vrp.x }
// { 0, 1, 0, -vrp.y }
// { 0, 0, 1, -vrp.z }
// { 0, 0, 0, 1 }
Mat4 Pipeline::A(const Vec3 &vrp) {

    return Mat4::translation(Vec3(-vrp.x, -vrp.y, -vrp.z));
}


Mat4 Pipeline::B(const Vec3 &u, const Vec3 &v, const Vec3 &n) {
    Mat4 m = Mat4::identity();

    m.m[0][0] = u.x; m.m[1][0] = u.y; m.m[2][0] = u.z; m.m[3][0] = 0.0;
    m.m[0][1] = v.x; m.m[1][1] = v.y; m.m[2][1] = v.z; m.m[3][1] = 0.0;
    m.m[0][2] = n.x; m.m[1][2] = n.y; m.m[2][2] = n.z; m.m[3][2] = 0.0;

    return m;
}

// C: centralizar janela de projeção
// { 1, 0, -Cu/d, 0 }
// { 0, 1, -Cv/d, 0 }
// { 0, 0,    1,  0 }
// { 0, 0,    0,  1 }
//
Mat4 Pipeline::C(double Cu, double Cv, double d) {
    Mat4 m = Mat4::identity();

    m.m[0][0] = 1.0;
    m.m[1][0] = 0.0;
    m.m[2][0] = -Cu / d;
    m.m[3][0] = 0.0;


    m.m[0][1] = 0.0;
    m.m[1][1] = 1.0;
    m.m[2][1] = -Cv / d;
    m.m[3][1] = 0.0;

    return m;
}

// D: normalizar escala
// { d/(Su*f),    0,    0, 0 }
// { 0,      d/(Sv*f),  0, 0 }
// { 0,          0,  1/f, 0 }
// { 0,          0,    0, 1 }
Mat4 Pipeline::D(double Su, double Sv, double d, double f) {
    Mat4 m = Mat4::identity();

    double sx = d / (Su * f);
    double sy = d / (Sv * f);
    double sz = 1.0 / f;

    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

// P: projeção em perspectiva
// { 1, 0, 0, 0 }
// { 0, 1, 0, 0 }
// { 0, 0, far/(far-near), -near/(far-near) }
// { 0, 0, 1, 0 }
Mat4 Pipeline::P(double farPlane, double nearPlane) {
    Mat4 m = Mat4::identity();

    double a = farPlane / (farPlane - nearPlane);
    double b = -nearPlane / (farPlane - nearPlane);

    // x' = x
    m.m[0][0] = 1.0;
    m.m[1][0] = 0.0;
    m.m[2][0] = 0.0;
    m.m[3][0] = 0.0;

    // y' = y
    m.m[0][1] = 0.0;
    m.m[1][1] = 1.0;
    m.m[2][1] = 0.0;
    m.m[3][1] = 0.0;

    // z' = a*z + b*w (w antigo = 1 geralmente)
    m.m[0][2] = 0.0;
    m.m[1][2] = 0.0;
    m.m[2][2] = a;
    m.m[3][2] = b;

    // w' = z
    m.m[0][3] = 0.0;
    m.m[1][3] = 0.0;
    m.m[2][3] = 1.0;
    m.m[3][3] = 0.0;

    return m;
}

// J: flip em Y
// { 1,  0, 0, 0 }
// { 0, -1, 0, 0 }
// { 0,  0, 1, 0 }
// { 0,  0, 0, 1 }
Mat4 Pipeline::J() {
    Mat4 m = Mat4::identity();
    m.m[1][1] = -1.0; // inverte Y
    return m;
}

// K: NDC [-1,1] -> [0,1] em x e y
// { 0.5, 0,   0, 0.5 }
// { 0,  0.5,  0, 0.5 }
// { 0,   0,   1, 0   }
// { 0,   0,   0, 1   }
Mat4 Pipeline::K() {
    Mat4 m = Mat4::identity();

    // x' = 0.5*x + 0.5*w
    m.m[0][0] = 0.5;
    m.m[1][0] = 0.0;
    m.m[2][0] = 0.0;
    m.m[3][0] = 0.5;

    // y' = 0.5*y + 0.5*w
    m.m[0][1] = 0.0;
    m.m[1][1] = 0.5;
    m.m[2][1] = 0.0;
    m.m[3][1] = 0.5;

    // z' = z
    m.m[0][2] = 0.0;
    m.m[1][2] = 0.0;
    m.m[2][2] = 1.0;
    m.m[3][2] = 0.0;

    // w' = w (deixa identidade na coluna 3)
    m.m[0][3] = 0.0;
    m.m[1][3] = 0.0;
    m.m[2][3] = 0.0;
    m.m[3][3] = 1.0;

    return m;
}

// L: [0,1] -> viewport em pixels
// { x_max - x_min, 0, 0, x_min }
// { 0, y_max - y_min, 0, y_min }
// { 0, 0, z_max - z_min, z_min }
// { 0, 0, 0, 1 }
Mat4 Pipeline::L(double x_max, double x_min,
                 double y_max, double y_min,
                 double z_max, double z_min) {
    Mat4 m = Mat4::identity();

    double sx = x_max - x_min;
    double sy = y_max - y_min;
    double sz = z_max - z_min;

    // x' = sx * x + x_min * w
    m.m[0][0] = sx;
    m.m[1][0] = 0.0;
    m.m[2][0] = 0.0;
    m.m[3][0] = x_min;

    // y' = sy * y + y_min * w
    m.m[0][1] = 0.0;
    m.m[1][1] = sy;
    m.m[2][1] = 0.0;
    m.m[3][1] = y_min;

    // z' = sz * z + z_min * w
    m.m[0][2] = 0.0;
    m.m[1][2] = 0.0;
    m.m[2][2] = sz;
    m.m[3][2] = z_min;

    // w' = w (coluna 3 = identidade)
    m.m[0][3] = 0.0;
    m.m[1][3] = 0.0;
    m.m[2][3] = 0.0;
    m.m[3][3] = 1.0;

    return m;
}

// M: shift adicional (0.5,0.5,0.5)
// { 1, 0, 0, 0.5 }
// { 0, 1, 0, 0.5 }
// { 0, 0, 1, 0.5 }
// { 0, 0, 0, 1 }
Mat4 Pipeline::M() {
    return Mat4::translation(Vec3(0.5, 0.5, 0.5));
}

