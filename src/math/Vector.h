#pragma once
#include <cmath>

//Estrutura que representa um vetor 3D com componentes x,y,z
struct Vec3 {
  double x{}, y{}, z{};//{} inicializa cada cordenada com 0.0

  Vec3() = default; //construtor padrao
  Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}//Construtor que recebe 3 doubles
  
  //Uma função que soma/subtrai/etc este Vec3 com outro Vec3 passado como referência 
  //constante, não altera nenhum dos dois, e retorna um novo Vec3 com o resultado.
  Vec3 operator+(const Vec3 &v) const { return {x + v.x, y + v.y, z + v.z}; }
  Vec3 operator-(const Vec3 &v) const { return {x - v.x, y - v.y, z - v.z}; }
  
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
  Vec3 operator/(double s) const { return {x / s, y / s, z / s}; }
  
  //Soma o vetor v ao vetor atual(modifica this)
  //Retorna uma referência para o próprio objeto para permitir encadeamento.
  Vec3 &operator+=(const Vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  
  //Produto escalar
  double dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
  
  //produto vetorial
  Vec3 cross(const Vec3 &v) const {
    return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
  }

  //Calculo do modulo(norma) do vetor
  double length() const { return std::sqrt(x * x + y * y + z * z); }

  //Retorna o vetor normalizado(vetor unitario)
  Vec3 normalized() const {
    double len = length();
    if (len == 0.0)//Evita divisao por 0
      return *this;
    return {x / len, y / len, z / len};
  }
};

//Estrutura do Vetor 4D
struct Vec4 {
  double x{}, y{}, z{}, w{};
  Vec4() = default;
  Vec4(double x_, double y_, double z_, double w_ = 1.0)
      : x(x_), y(y_), z(z_), w(w_) {}
};
