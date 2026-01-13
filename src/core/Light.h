#pragma once
#include "../math/Vector.h"

// Estrutura de luz pontual
struct Light {
    Vec3 position;      // Posição da luz no espaço mundo
    Vec3 color;         // Cor da luz (RGB em [0,1])
    double intensity;   // Intensidade da luz (multiplicador)
    
    // Construtor padrão
    Light() : position{0, 0, 0}, color{1, 1, 1}, intensity{1.0} {}
    
    // Construtor com parâmetros
    Light(const Vec3& pos, const Vec3& col, double intens = 1.0)
        : position(pos), color(col), intensity(intens) {}
};

