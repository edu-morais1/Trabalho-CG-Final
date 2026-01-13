#include "src/core/Scene.h"
#include "src/pipeline/Rasterizer.h"
#include <cstdint>
#include <fstream>
#include <iostream>

// Função que Python chamará via pybind11 ou ctypes
extern "C" void render_api(int width, int height, double eye_x, double eye_y,
                           double eye_z, double center_x, double center_y,
                           double center_z, uint32_t *out_pixels) {
  // Criar cena
  Scene scene;

  // Configurar câmera
  scene.camera.eye = Vec3{eye_x, eye_y, eye_z};
  scene.camera.center = Vec3{center_x, center_y, center_z};
  scene.camera.up = Vec3{0, 1, 0};
  scene.camera.fovY = 60.0;
  scene.camera.aspect = (double)width / height;
  scene.camera.nearPlane = 0.1;
  scene.camera.farPlane = 100.0;

  // Adicionar um cubo de exemplo
  Cube cube;
  cube.position = Vec3{0, 0, 0};
  cube.rotation = Vec3{0, 0, 0};
  cube.scale = 1.0;
  cube.material.color = Vec3{0.8, 0.2, 0.2}; // vermelho
  cube.material.ka = 0.1;
  cube.material.kd = 0.7;
  cube.material.ks = 0.3;
  cube.material.shininess = 32.0;
  scene.cubes.push_back(cube);

  // Adicionar luz
  Light light;
  light.position = Vec3{5, 5, 5};
  light.color = Vec3{1, 1, 1};
  light.intensity = 1.0;
  scene.lights.push_back(light);

  // Criar framebuffer e renderizar
  Framebuffer fb(width, height);
  fb.clear(0xff000000);
  renderScene(scene, fb, true); // true = Phong shading

  // Copiar pixels para saída
  std::copy(fb.color.begin(), fb.color.end(), out_pixels);
}

// Função main para teste standalone (gera imagem PPM)
int main() {
  const int width = 800;
  const int height = 600;

  // Criar cena
  Scene scene;

  // Configurar câmera
  scene.camera.eye = Vec3{3, 2, 4};
  scene.camera.center = Vec3{0, 0, 0};
  scene.camera.up = Vec3{0, 1, 0};
  scene.camera.fovY = 60.0;
  scene.camera.aspect = (double)width / height;
  scene.camera.nearPlane = 0.1;
  scene.camera.farPlane = 100.0;

  // Adicionar cubos coloridos
  Cube cube1;
  cube1.position = Vec3{-1.5, 0, 0};
  cube1.rotation = Vec3{0, 0.5, 0};
  cube1.scale = 1.0;
  cube1.material.color = Vec3{0.8, 0.2, 0.2}; // vermelho
  cube1.material.ka = 0.1;
  cube1.material.kd = 0.7;
  cube1.material.ks = 0.3;
  cube1.material.shininess = 32.0;
  scene.cubes.push_back(cube1);

  Cube cube2;
  cube2.position = Vec3{1.5, 0, 0};
  cube2.rotation = Vec3{0.3, 0, 0};
  cube2.scale = 1.0;
  cube2.material.color = Vec3{0.2, 0.8, 0.2}; // verde
  cube2.material.ka = 0.1;
  cube2.material.kd = 0.7;
  cube2.material.ks = 0.3;
  cube2.material.shininess = 32.0;
  scene.cubes.push_back(cube2);

  Cube cube3;
  cube3.position = Vec3{0, 1.5, 0};
  cube3.rotation = Vec3{0, 0, 0.3};
  cube3.scale = 0.8;
  cube3.material.color = Vec3{0.2, 0.2, 0.8}; // azul
  cube3.material.ka = 0.1;
  cube3.material.kd = 0.7;
  cube3.material.ks = 0.5;
  cube3.material.shininess = 64.0;
  scene.cubes.push_back(cube3);

  // Adicionar luzes
  Light light1;
  light1.position = Vec3{5, 5, 5};
  light1.color = Vec3{1, 1, 1};
  light1.intensity = 1.0;
  scene.lights.push_back(light1);

  Light light2;
  light2.position = Vec3{-3, 2, 3};
  light2.color = Vec3{0.5, 0.5, 0.8};
  light2.intensity = 0.5;
  scene.lights.push_back(light2);

  // Criar framebuffer
  Framebuffer fb(width, height);
  fb.clear(0xff111111); // cinza escuro

  // Renderizar
  std::cout << "Renderizando cena " << width << "x" << height << "..."
            << std::endl;
  renderScene(scene, fb, true); // true = Phong shading
  std::cout << "Renderização completa!" << std::endl;

  // Salvar imagem PPM
  std::ofstream file("output.ppm", std::ios::binary);
  file << "P6\n" << width << " " << height << "\n255\n";

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      uint32_t pixel = fb.color[y * width + x];
      uint8_t r = (pixel >> 16) & 0xff;
      uint8_t g = (pixel >> 8) & 0xff;
      uint8_t b = pixel & 0xff;
      file.put(r).put(g).put(b);
    }
  }

  file.close();
  std::cout << "Imagem salva em 'output.ppm'" << std::endl;
  std::cout << "Converta para PNG com: convert output.ppm output.png"
            << std::endl;

  return 0;
}
