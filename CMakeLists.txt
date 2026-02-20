cmake_minimum_required(VERSION 3.16)
project(TrabalhoCG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

set(SOURCES
    src/core/Camera.cpp
    src/core/Cube.cpp
    src/pipeline/Pipeline.cpp
    src/pipeline/Shading.cpp
    src/pipeline/Rasterizer.cpp
    main.cpp
)

# Biblioteca compartilhada (carregada pelo Python via ctypes)
add_library(render SHARED ${SOURCES})

target_compile_options(render PRIVATE -Wall -Wextra -O2)

