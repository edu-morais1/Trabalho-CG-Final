#pragma once
#include "Camera.h"
#include "Cube.h"
#include "Light.h"
#include <vector>

struct Scene {
  Camera camera;
  std::vector<Cube> cubes;
  std::vector<Light> lights;
};
