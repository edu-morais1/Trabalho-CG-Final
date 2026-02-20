#include "src/core/Scene.h"
#include "src/pipeline/Rasterizer.h"
#include <cstdint>
#include <cstring>

// Função que Python chamará via ctypes
extern "C" {
void render_api(
    int width, int height,
    // Câmera
    double eye_x, double eye_y, double eye_z, double center_x, double center_y,
    double center_z, double fov, double near_plane, double far_plane,
    // Cubos (máximo 10 cubos, 13 params cada)
    int num_cubes,
    double *cubes_data, // [pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, scale,
                        // color.r, color.g, color.b, ka, kd, ks]
    // Luzes (máximo 5 luzes, 7 params cada)
    int num_lights,
    double *lights_data, // [pos.x, pos.y, pos.z, color.r, color.g, color.b,
                         // intensity]
    // Shading
    int use_phong,
    // Saída
    uint32_t *out_pixels) {
  // Criar cena
  Scene scene;

  // Configurar câmera
  scene.camera.eye = Vec3{eye_x, eye_y, eye_z};
  scene.camera.center = Vec3{center_x, center_y, center_z};
  scene.camera.up = Vec3{0, 1, 0};
  scene.camera.fovY = fov;
  scene.camera.aspect = (double)width / height;
  scene.camera.nearPlane = near_plane;
  scene.camera.farPlane = far_plane;

  scene.camera.updateUVN();

  // Adicionar cubos
  for (int i = 0; i < num_cubes; ++i) {
    double *data = &cubes_data[i * 13];
    Cube cube;
    cube.position = Vec3{data[0], data[1], data[2]};
    cube.rotation = Vec3{data[3], data[4], data[5]};
    cube.scale = data[6];
    cube.material.color = Vec3{data[7], data[8], data[9]};
    cube.material.ka = data[10];
    cube.material.kd = data[11];
    cube.material.ks = data[12];
    cube.material.shininess = 32.0;
    scene.cubes.push_back(cube);
  }

  // Adicionar luzes
  for (int i = 0; i < num_lights; ++i) {
    double *data = &lights_data[i * 7];
    Light light;
    light.position = Vec3{data[0], data[1], data[2]};
    light.color = Vec3{data[3], data[4], data[5]};
    light.intensity = data[6];
    scene.lights.push_back(light);
  }

  // Renderizar
  Framebuffer fb(width, height);
  fb.clear(0xff1a1a1a);
  renderScene(scene, fb, use_phong != 0);

  // Copiar pixels
  std::memcpy(out_pixels, fb.color.data(), width * height * sizeof(uint32_t));
}
}
