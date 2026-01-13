#pragma once
#include "../core/Cube.h"
#include "../core/Light.h"
#include "../math/Vector.h"
#include <vector>

// Flat shading: usa normal da face
Vec3 flatShading(const Vec3 &normal, const Vec3 &position, const Material &mat,
                 const Light &light, const Vec3 &eyePos);

// Phong shading: interpola normais por pixel
Vec3 phongShading(const Vec3 &normal, const Vec3 &position, const Material &mat,
                  const Light &light, const Vec3 &eyePos);

// Versão com múltiplas luzes
Vec3 computeLighting(const Vec3 &normal, const Vec3 &position,
                     const Material &mat, const std::vector<Light> &lights,
                     const Vec3 &eyePos, bool usePhong = false);
