# 🎨 Trabalho Final - Computação Gráfica

Pipeline de renderização 3D implementado do zero em C++17, com interface gráfica Python (Tkinter) para edição de cenas em tempo real.

## ✨ Características Implementadas

### Requisitos Atendidos

- ✅ **Requisito 1**: Modelar cenas 3D compostas por múltiplos cubos
- ✅ **Requisito 2**: Apresentação em projeção perspectiva
- ✅ **Requisito 3**: Seleção e transformação geométrica (translação, rotação, escala)
- ✅ **Requisito 4**: Sombreamento constante (Flat Shading) + z-buffer
- ✅ **Requisito 5**: Sombreamento Phong + z-buffer
- ✅ **Requisito 6**: **Parâmetros editáveis em tempo de execução via interface gráfica**
- ✅ **Requisito 7**: Pipeline baseado em Alvy Ray Smith

### Funcionalidades Técnicas

- **Pipeline Gráfico Completo**: Model → View → Projection → NDC → Screen
- **Rasterização**: Algoritmo scanline com coordenadas baricêntricas
- **Z-buffer**: Teste de profundidade para ocultação correta de superfícies
- **Iluminação Phong**: Componentes ambiente, difusa e especular
- **Iluminação Flat**: Sombreamento constante por face
- **Back-face Culling**: Otimização de renderização
- **Múltiplas Luzes**: Suporte a várias fontes de luz simultâneas
- **Câmera Look-at**: Implementação baseada em Alvy Ray Smith

## 🏗️ Estrutura do Projeto

```
Trabalho-CG-Final/
├── src/
│   ├── core/
│   │   ├── Camera.h / Camera.cpp      # Sistema de câmera
│   │   ├── Cube.h / Cube.cpp          # Geometria do cubo
│   │   ├── Light.h                     # Fonte de luz
│   │   └── Scene.h                     # Estrutura da cena
│   ├── math/
│   │   ├── Vector.h                    # Vetores 3D e 4D
│   │   └── Matrix.h                    # Matrizes 4x4
│   └── pipeline/
│       ├── Rasterizer.h / .cpp        # Rasterização e z-buffer
│       ├── Shading.h / .cpp           # Modelos de iluminação
│       └── Transform.h                 # Transformações geométricas
├── main.cpp                            # API C++ para Python
├── gui_tkinter.py                      # Interface gráfica
└── README.md
```

## 🔧 Instalação e Execução

### Pré-requisitos

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install g++ python3 python3-tk python3-pip
```

#### Linux (Arch/Manjaro)
```bash
sudo pacman -S gcc python tk python-pip
```

#### macOS
```bash
brew install gcc python-tk
```

#### Windows
- Instalar [MinGW-w64](https://www.mingw-w64.org/)
- Instalar [Python 3.8+](https://www.python.org/downloads/) (Tkinter vem incluído)

### Instalação

**1. Clone o repositório**
```bash
git clone https://github.com/edu-morais1/Trabalho-CG-Final.git
cd Trabalho-CG-Final
```

**2. Compile o backend C++**

Linux:
```bash
g++ -std=c++17 -shared -fPIC -o librender.so \
    src/core/Camera.cpp \
    src/core/Cube.cpp \
    src/pipeline/Shading.cpp \
    src/pipeline/Rasterizer.cpp \
    main.cpp \
    -Isrc -O2 -Wall
```

Windows (MinGW):
```bash
g++ -std=c++17 -shared -o render.dll \
    src/core/Camera.cpp \
    src/core/Cube.cpp \
    src/pipeline/Shading.cpp \
    src/pipeline/Rasterizer.cpp \
    main.cpp \
    -Isrc -O2 -Wall
```

macOS:
```bash
g++ -std=c++17 -shared -o librender.dylib \
    src/core/Camera.cpp \
    src/core/Cube.cpp \
    src/pipeline/Shading.cpp \
    src/pipeline/Rasterizer.cpp \
    main.cpp \
    -Isrc -O2 -Wall
```

**3. Instale dependências Python**
```bash
pip install pillow numpy
```

**4. Execute a interface gráfica**
```bash
python gui_tkinter.py
```

## 🎮 Manual de Uso

### Interface Gráfica

A interface está dividida em duas seções principais:

#### Visualização 3D (Esquerda)
- Canvas de 800x600 pixels mostrando a cena renderizada
- Atualiza automaticamente ao modificar parâmetros

#### Painel de Controles (Direita)

**🎥 Câmera**
- **Eye X/Y/Z**: Posição da câmera no espaço 3D
- **Center X/Y/Z**: Ponto focal (para onde a câmera aponta)
- **FOV**: Campo de visão (30° a 120°)

**📦 Seleção de Cubos**
- **Cubo 1/2/3**: Botões para selecionar cubo ativo
- **+ Adicionar**: Adiciona novo cubo à cena
- **- Remover**: Remove cubo selecionado (mínimo 1)

**🔧 Transformações do Cubo Selecionado**
- **Posição X/Y/Z**: Translação nos eixos (-5 a 5)
- **Rotação X/Y/Z**: Rotação em radianos (-π a π)
- **Escala**: Escala uniforme (0.1 a 3.0)

**🎨 Material do Cubo**
- **Cor (R/G/B)**: Cor base do material (0.0 a 1.0)
- **Ka**: Coeficiente de reflexão ambiente
- **Kd**: Coeficiente de reflexão difusa
- **Ks**: Coeficiente de reflexão especular

**💡 Iluminação**
- **Luz 1 X/Y/Z**: Posição da primeira fonte de luz
- **Cor da Luz**: Cor RGB da luz

**🖌️ Renderização**
- **☑ Phong Shading**: Ativa/desativa sombreamento Phong
  - Marcado: Phong Shading (interpolação de normais)
  - Desmarcado: Flat Shading (cor constante por face)
- **🔄 Renderizar**: Atualiza a visualização

### Exemplos de Uso

**Mover câmera ao redor da cena:**
1. Ajuste Eye X, Y, Z para orbitar os objetos
2. Mantenha Center em (0, 0, 0) para focar na origem

**Adicionar e transformar cubos:**
1. Clique em "+ Adicionar" para novo cubo
2. Selecione o cubo criado
3. Ajuste Posição para mover
4. Ajuste Rotação para girar
5. Ajuste Escala para redimensionar

**Comparar Flat vs Phong:**
1. Configure a cena
2. Desmarque "Phong Shading" → Flat
3. Marque "Phong Shading" → Phong
4. Observe a diferença na suavidade

## 🎓 Conceitos Implementados

### Pipeline de Renderização

```
Espaço do Objeto → Espaço do Mundo → Espaço da Câmera → 
Espaço de Recorte → NDC → Coordenadas de Tela
```

1. **Transformação de Modelo**: Aplica translação, rotação e escala
2. **Transformação de Visualização**: Matriz look-at (Alvy Ray Smith)
3. **Transformação de Projeção**: Projeção perspectiva
4. **Divisão Perspectiva**: Normalização para NDC
5. **Mapeamento de Viewport**: Conversão para coordenadas de tela

### Rasterização

- **Coordenadas Baricêntricas**: Interpolação de atributos dentro de triângulos
- **Scanline**: Varredura eficiente de pixels
- **Z-buffer**: Profundidade por pixel para ocultação correta

### Modelos de Iluminação

**Flat Shading:**
- Normal constante por face
- Iluminação calculada uma vez por triângulo
- Visual facetado

**Phong Shading:**
- Normais interpoladas por pixel
- Iluminação calculada por pixel
- Visual suavizado

**Modelo de Iluminação Phong:**
\[ I = I_a k_a + I_d k_d (\mathbf{N} \cdot \mathbf{L}) + I_s k_s (\mathbf{R} \cdot \mathbf{V})^n \]

Onde:
- \(I_a\): Luz ambiente
- \(I_d\): Luz difusa
- \(I_s\): Luz especular
- \(k_a, k_d, k_s\): Coeficientes do material
- \(\mathbf{N}\): Normal
- \(\mathbf{L}\): Direção da luz
- \(\mathbf{R}\): Reflexão
- \(\mathbf{V}\): Direção da câmera

## 🐛 Solução de Problemas

### Erro: `cannot open shared object file`
```bash
# Linux: adicione diretório atual
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
python gui_tkinter.py
```

### Erro: `ImportError: libtk8.6.so`
```bash
# Ubuntu/Debian
sudo apt install python3-tk

# Arch Linux
sudo pacman -S tk
```

### Tela preta ou não renderiza
Verifique se a biblioteca foi compilada:
```bash
ls -lh librender.so  # Linux
ls -lh render.dll    # Windows
ls -lh librender.dylib  # macOS
```

### Renderização muito lenta
- Reduza a resolução no código (modifique `self.width` e `self.height`)
- Use menos cubos na cena
- Compile com otimização `-O3`

## 📚 Referências

1. **Alvy Ray Smith** - "A Pixel Is Not A Little Square" e "The Viewing Transformation"
   - [https://alvyray.com/Memos/CG/Microsoft/6_pixel.pdf](https://alvyray.com/Memos/CG/Microsoft/6_pixel.pdf)

2. **Shirley, P.; Marschner, S.** - "Fundamentals of Computer Graphics" (5th Edition)

3. **Akenine-Möller, T.; Haines, E.; Hoffman, N.** - "Real-Time Rendering" (4th Edition)

4. **Foley, J. D.; van Dam, A.** - "Computer Graphics: Principles and Practice"

## 👥 Autores

**Eduardo Morais**
- GitHub: [@edu-morais1](https://github.com/edu-morais1)
- Email: [seu-email@exemplo.com]

## 📝 Informações Acadêmicas

- **Instituição**: Universidade Estadual do Oeste do Paraná (UNIOESTE)
- **Campus**: Cascavel
- **Curso**: Ciência da Computação
- **Disciplina**: Computação Gráfica
- **Professor**: Adair Santa Catarina
- **Período**: 2025
- **Data de Entrega**: 09/02/2026
- **Data de Apresentação**: 12/02/2026

## 📄 Licença

Este projeto foi desenvolvido para fins acadêmicos como requisito da disciplina de Computação Gráfica.

---

## 📊 Estatísticas do Projeto

- **Linhas de Código C++**: ~800
- **Linhas de Código Python**: ~250
- **Arquivos**: 15
- **Tempo de Desenvolvimento**: 2 semanas

