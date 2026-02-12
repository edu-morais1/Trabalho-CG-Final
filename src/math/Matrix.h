#pragma once
#include "Vector.h"

//Struct Mat4, que representa uma matriz 4x4 de doubles
struct Mat4 {
  //Erray bi dimensional 4x4({} zera por padrao)
  double m[4][4]{};

  //Matriz identidade
  static Mat4 identity() {
    Mat4 r{};
    for (int i = 0; i < 4; ++i)
      r.m[i][i] = 1.0;
    return r;
  }
  
  //Matriz de Translacao
  static Mat4 translation(const Vec3 &t) {
    Mat4 r = identity();
    r.m[3][0] = t.x;
    r.m[3][1] = t.y;
    r.m[3][2] = t.z;
    return r;
  }
  
  //Funcao que cria a martriz de escala
  static Mat4 scale(double s) {
    Mat4 r{};
    r.m[0][0] = r.m[1][1] = r.m[2][2] = s;
    r.m[3][3] = 1.0;
    return r;
  }

  //Funcao que cria a matriz de rotacao em torno de X
  static Mat4 rotationX(double rad) {
    Mat4 r = identity();
    double c = std::cos(rad), s = std::sin(rad);
    r.m[1][1] = c;
    r.m[1][2] = s;
    r.m[2][1] = -s;
    r.m[2][2] = c;
    return r;
  }
  
  //Funcao que cria a matriz de rotacao em torno de Y
  static Mat4 rotationY(double rad) {
    Mat4 r = identity();
    double c = std::cos(rad), s = std::sin(rad);
    r.m[0][0] = c;
    r.m[0][2] = -s;
    r.m[2][0] = s;
    r.m[2][2] = c;
    return r;
  }

  //Funcao que cria a matriz de rotacao em torno de Z
  static Mat4 rotationZ(double rad) {
    Mat4 r = identity();
    double c = std::cos(rad), s = std::sin(rad);
    r.m[0][0] = c;
    r.m[0][1] = s;
    r.m[1][0] = -s;
    r.m[1][1] = c;
    return r;
  }
  
  
  Mat4 operator*(const Mat4 &o) const {
    Mat4 r{};
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        for (int k = 0; k < 4; ++k)
          r.m[i][j] += m[i][k] * o.m[k][j];
    return r;
  }

  Vec4 operator*(const Vec4 &v) const {
    Vec4 r;
    r.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
    r.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
    r.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
    r.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
    return r;
  }
};
