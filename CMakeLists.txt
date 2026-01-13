cmake_minimum_required(VERSION 3.16)
project(TrabalhoCG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Diretórios de include
include_directories(${CMAKE_SOURCE_DIR})

# Fontes
set(SOURCES
    src/core/Camera.cpp
    src/core/Cube.cpp
    src/pipeline/Shading.cpp
    src/pipeline/Rasterizer.cpp
    main.cpp
)

# Executável
add_executable(render ${SOURCES})

# Para debug
target_compile_options(render PRIVATE -Wall -Wextra -g)

