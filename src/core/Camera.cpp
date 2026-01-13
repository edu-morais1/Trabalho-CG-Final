#include "Camera.h"
#include <cmath>

Mat4 Camera::viewMatrix() const {
  Vec3 n = (center - eye).normalized(); // direção de visão
  Vec3 u = n.cross(up).normalized();    // eixo "right"
  Vec3 v = u.cross(n);                  // eixo "up" ajustado

  Mat4 R = Mat4::identity();
  R.m[0][0] = u.x;
  R.m[1][0] = u.y;
  R.m[2][0] = u.z;
  R.m[0][1] = v.x;
  R.m[1][1] = v.y;
  R.m[2][1] = v.z;
  R.m[0][2] = -n.x;
  R.m[1][2] = -n.y;
  R.m[2][2] = -n.z;

  Mat4 T = Mat4::translation({-eye.x, -eye.y, -eye.z});
  return T * R; // mundo → view
}

Mat4 Camera::projectionMatrix() const {
  double fovyRad = fovY * M_PI / 180.0;
  double t = std::tan(fovyRad / 2.0);
  double n = nearPlane;
  double f = farPlane;

  Mat4 P{};
  P.m[0][0] = 1.0 / (aspect * t);
  P.m[1][1] = 1.0 / t;
  P.m[2][2] = f / (f - n);
  P.m[2][3] = 1.0;
  P.m[3][2] = -n * f / (f - n);
  return P; // mesma ideia do P do Alvy [file:5]
}
