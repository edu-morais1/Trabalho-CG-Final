#pragma once
#include "../math/Vector.h"
#include "../math/Matrix.h"

// ViewStructure do Alvy Ray Smith (simplificada)
struct Camera {
    Vec3 eye;        // ViewPoint posicao da camera no mundo
    Vec3 center;     // ponto que a camera olha 
    Vec3 up;         // ViewUp orientacao "pra cima"

    double nearPlane{1.0};//distancia do plano de recorte proximo(near)
    double farPlane{100.0};
    double fovY{60.0};       // Campo de visao vertical em graus
    double aspect{4.0/3.0};  // width/height

    // Matriz view (equivalente a A*B do Alvy, na prática um look-at)
    // rotacionar + transladar
    Mat4 viewMatrix() const;

    // Matriz de projeção perspectiva
    Mat4 projectionMatrix() const;
};

