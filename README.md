# 🎨 Trabalho Final - Computação Gráfica

Pipeline de renderização 3D implementado do zero em C++17, com suporte a transformações geométricas, rasterização, z-buffer, e modelos de iluminação Phong e Flat Shading.

## ✨ Características

- **Pipeline completo**: Model → View → Projection → NDC → Screen
- **Rasterização**: Algoritmo scanline com coordenadas baricêntricas
- **Z-buffer**: Teste de profundidade para visibilidade correta
- **Iluminação**:
  - Flat Shading (cor constante por face)
  - Phong Shading (interpolação de normais por pixel)
- **Back-face culling**: Otimização de renderização
- **Multi-luzes**: Suporte a múltiplas fontes de luz
- **Transformações**: Translação, rotação, escala
- **Câmera look-at**: Baseada no artigo de Alvy Ray Smith

## 🏗️ Estrutura do Projeto

src/
├── core/ # Componentes da cena (Camera, Cube, Light, Scene)
├── math/ # Vetores e matrizes 3D/4D
└── pipeline/ # Rasterização, transformações, shading
main.cpp # Aplicação de teste

text

## 🔧 Compilação

### Com g++:
```bash
g++ -std=c++17 -o render \
    src/core/Camera.cpp \
    src/core/Cube.cpp \
    src/pipeline/Shading.cpp \
    src/pipeline/Rasterizer.cpp \
    main.cpp \
    -Isrc -O2 -Wall
Com CMake (opcional):
bash
mkdir build && cd build
cmake ..
make
🚀 Execução
bash
./render
Gera output.ppm (formato PPM). Converta para PNG:

bash
convert output.ppm output.png
# ou
magick output.ppm output.png
📚 Referências
The Viewing Transformation - Alvy Ray Smith

Fundamentals of Computer Graphics (Shirley & Marschner)

Real-Time Rendering (Akenine-Möller et al.)

👥 Autores
Eduardo Morais - @edu-morais1

📝 Licença
Este projeto é para fins acadêmicos.
