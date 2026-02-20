#include "Shading.h"
#include <algorithm>
#include <cmath>

// Lógica central de iluminação Blinn-Phong (usada por flat e phong)
static Vec3 blinnPhong(const Vec3 &N, const Vec3 &position,
                       const Material &mat, const Light &light,
                       const Vec3 &eyePos) {
    Vec3 Nn = N.normalized();
    Vec3 L  = (light.position - position).normalized();
    Vec3 V  = (eyePos - position).normalized();

    // Blinn-Phong: vetor H no lugar de R
    Vec3 H = (L + V).normalized();

    // Ambiente
    Vec3 ambient = mat.color * mat.ka;

    // Difusa (Lambert)
    double diff = std::max(0.0, Nn.dot(L));
    Vec3 diffuse = mat.color * (mat.kd * diff);

    // Especular Blinn-Phong: N·H  (era R·V no Phong clássico)
    double dotNH = std::max(0.0, Nn.dot(H));
    double spec  = std::pow(dotNH, mat.shininess);
    Vec3 specular = light.color * (mat.ks * spec * light.intensity);

    Vec3 result = ambient + (diffuse + specular) * light.intensity;
    result.x = std::clamp(result.x, 0.0, 1.0);
    result.y = std::clamp(result.y, 0.0, 1.0);
    result.z = std::clamp(result.z, 0.0, 1.0);
    return result;
}

Vec3 flatShading(const Vec3 &normal, const Vec3 &position,
                 const Material &mat, const Light &light,
                 const Vec3 &eyePos) {
    return blinnPhong(normal, position, mat, light, eyePos);
}

Vec3 phongShading(const Vec3 &normal, const Vec3 &position,
                  const Material &mat, const Light &light,
                  const Vec3 &eyePos) {
    // Chamado por pixel com normal interpolada — mesma fórmula
    return blinnPhong(normal, position, mat, light, eyePos);
}

Vec3 computeLighting(const Vec3 &normal, const Vec3 &position,
                     const Material &mat, const std::vector<Light> &lights,
                     const Vec3 &eyePos, bool usePhong) {
    Vec3 color{0, 0, 0};
    for (const auto &light : lights) {
        if (usePhong)
            color += phongShading(normal, position, mat, light, eyePos);
        else
            color += flatShading(normal, position, mat, light, eyePos);
    }
    color.x = std::clamp(color.x, 0.0, 1.0);
    color.y = std::clamp(color.y, 0.0, 1.0);
    color.z = std::clamp(color.z, 0.0, 1.0);
    return color;
}

