#pragma once
#include "../math/Vector.h"
#include "../math/Matrix.h"

// ViewStructure do Alvy Ray Smith (simplificada)
struct Camera {
    Vec3 eye;        // ViewPoint
    Vec3 center;     // ponto que a camera olha
    Vec3 up;         // ViewUp

    double nearPlane{1.0};
    double farPlane{100.0};
    double fovY{60.0};       // em graus
    double aspect{4.0/3.0};  // width/height

    // Matriz view (equivalente a A*B do Alvy, na prática um look-at)
    Mat4 viewMatrix() const;

    // Matriz de projeção perspectiva
    Mat4 projectionMatrix() const;
};

