#pragma once
#include "../math/Vector.h"
#include "../math/Matrix.h"
#include <cmath>

struct Camera {
    Vec3 eye;
    Vec3 center;
    Vec3 up;

    double nearPlane { 1.0  };
    double farPlane  { 100.0};
    double fovY      { 60.0 };
    double aspect    { 4.0/3.0 };

    Vec3 vrp;
    Vec3 u;
    Vec3 v;
    Vec3 n;
    
    double DP { 1.0 };
    double Cu { 0.0 };
    double Cv { 0.0 };
    double Su { 1.0 };
    double Sv { 1.0 };
    
    double zMin { 0.0 };
    double zMax { 1.0 };

    void updateUVN() {
    vrp = eye;

    // forward = direção que a câmera olha (centro - olho)
    Vec3 forward = (center - eye).normalized();

    // n de Alvy Ray Smith = aponta para FORA da cena (olho - centro)
    // Isso é o que o Camera.cpp original usava como "-n" no eixo Z da view
    n = Vec3{-forward.x, -forward.y, -forward.z};

    // u e v: mesma lógica do Camera.cpp original
    u = forward.cross(up).normalized();
    v = u.cross(forward);

    // Parâmetros da janela de projeção
    DP = nearPlane;
    double halfH = nearPlane * std::tan(fovY * M_PI / 180.0 / 2.0);
    Sv = halfH;
    Su = aspect * halfH;
    Cu = 0.0;
    Cv = 0.0;
    zMin = 0.0;
    zMax = 1.0;
  }

        
    Mat4 viewMatrix() const;

    Mat4 projectionMatrix() const;
};

