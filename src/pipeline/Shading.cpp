#include "Shading.h"
#include <algorithm>
#include <cmath>

// Flat shading: usa normal da face, calcula iluminação uma vez
Vec3 flatShading(const Vec3 &normal, const Vec3 &position, const Material &mat,
                 const Light &light, const Vec3 &eyePos) {
  Vec3 N = normal.normalized();
  Vec3 L = (light.position - position).normalized();
  Vec3 V = (eyePos - position).normalized();
  Vec3 R = N * (2.0 * N.dot(L)) - L; // reflexão

  // Componente ambiente
  Vec3 ambient = mat.color * mat.ka;

  // Componente difusa (Lambert)
  double diff = std::max(0.0, N.dot(L));
  Vec3 diffuse = mat.color * (mat.kd * diff);

  // Componente especular (Blinn-Phong)
  double spec = std::pow(std::max(0.0, R.dot(V)), mat.shininess);
  Vec3 specular = light.color * (mat.ks * spec);

  // Soma das contribuições (multiplicar por intensidade da luz)
  Vec3 result = ambient + diffuse + specular;

  // Clamp para [0,1]
  result.x = std::clamp(result.x, 0.0, 1.0);
  result.y = std::clamp(result.y, 0.0, 1.0);
  result.z = std::clamp(result.z, 0.0, 1.0);

  return result;
}

// Phong shading: interpola normais e calcula iluminação por pixel
Vec3 phongShading(const Vec3 &normal, const Vec3 &position, const Material &mat,
                  const Light &light, const Vec3 &eyePos) {
  // Mesma lógica do flat, mas será chamado por pixel
  // com normais interpoladas baricêntricamente
  return flatShading(normal, position, mat, light, eyePos);
}

// Versão com múltiplas luzes
Vec3 computeLighting(const Vec3 &normal, const Vec3 &position,
                     const Material &mat, const std::vector<Light> &lights,
                     const Vec3 &eyePos, bool usePhong) {
  Vec3 color{0, 0, 0};

  // Acumular contribuição de cada luz
  for (const auto &light : lights) {
    if (usePhong) {
      color += phongShading(normal, position, mat, light, eyePos);
    } else {
      color += flatShading(normal, position, mat, light, eyePos);
    }
  }

  // Clamp resultado final
  color.x = std::clamp(color.x, 0.0, 1.0);
  color.y = std::clamp(color.y, 0.0, 1.0);
  color.z = std::clamp(color.z, 0.0, 1.0);

  return color;
}
