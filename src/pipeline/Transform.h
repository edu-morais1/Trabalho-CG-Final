#pragma once
#include "../math/Matrix.h"
#include "../math/Vector.h"

// Converter Vec3 para Vec4 homogêneo (SEM argumento padrão aqui)
inline Vec4 toVec4(const Vec3 &v, double w) { return Vec4{v.x, v.y, v.z, w}; }

// Sobrecarga para converter Vec3 para Vec4 (w=1 por padrão)
inline Vec4 toVec4(const Vec3 &v) { return Vec4{v.x, v.y, v.z, 1.0}; }

// Converter Vec4 para Vec3 (divisão por w)
inline Vec3 fromVec4(const Vec4 &v) {
  if (std::abs(v.w) < 1e-10)
    return Vec3{0, 0, 0};
  return Vec3{v.x / v.w, v.y / v.w, v.z / v.w};
}

// Transformar ponto para NDC
inline Vec3 transformToNDC(const Vec3 &p, const Mat4 &model, const Mat4 &view,
                           const Mat4 &proj) {
  // Ordem correta: proj * view * model * ponto
  Vec4 worldPos = model * toVec4(p);
  Vec4 viewPos = view * worldPos;
  Vec4 clipPos = proj * viewPos;

  // Divisão perspectiva
  if (std::abs(clipPos.w) < 1e-10)
    return Vec3{0, 0, 0};

  return Vec3{clipPos.x / clipPos.w, clipPos.y / clipPos.w,
              clipPos.z / clipPos.w};
}
