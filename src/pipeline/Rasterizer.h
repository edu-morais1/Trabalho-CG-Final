#pragma once
#include "../core/Scene.h"
#include <cstdint>
#include <vector>

// Framebuffer com z-buffer
struct Framebuffer {
  int width, height;
  std::vector<uint32_t> color;
  std::vector<double> depth;

  Framebuffer(int w, int h);
  void clear(uint32_t c);
  void putPixel(int x, int y, double z, const Vec3 &col);
};

// Fun├º├Áes principais
void renderScene(const Scene &scene, Framebuffer &fb, bool usePhong);
