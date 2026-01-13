#pragma once
#include <cmath>

struct Vec3 {
  double x{}, y{}, z{};

  Vec3() = default;
  Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

  Vec3 operator+(const Vec3 &v) const { return {x + v.x, y + v.y, z + v.z}; }
  Vec3 operator-(const Vec3 &v) const { return {x - v.x, y - v.y, z - v.z}; }
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
  Vec3 operator/(double s) const { return {x / s, y / s, z / s}; }

  Vec3 &operator+=(const Vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  double dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  Vec3 cross(const Vec3 &v) const {
    return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
  }

  double length() const { return std::sqrt(x * x + y * y + z * z); }

  Vec3 normalized() const {
    double len = length();
    if (len == 0.0)
      return *this;
    return {x / len, y / len, z / len};
  }
};

struct Vec4 {
  double x{}, y{}, z{}, w{};
  Vec4() = default;
  Vec4(double x_, double y_, double z_, double w_ = 1.0)
      : x(x_), y(y_), z(z_), w(w_) {}
};
