#pragma once
#include "../math/Vector.h"
#include "../math/Matrix.h"

struct Material {
    Vec3 color{1.0, 1.0, 1.0};  // base
    double ka{0.1};
    double kd{0.8};
    double ks{0.5};
    double shininess{32.0};//Espoente
};

struct Cube {
    Vec3 position;
    Vec3 rotation; // em radianos
    double scale{1.0};
    Material material;

    // 8 vértices do cubo unitário (centrado na origem SRU)
    static void baseVertices(Vec3 out[8]);

    // Devolve matriz modelo (S * R * T)
    Mat4 modelMatrix() const;
};

