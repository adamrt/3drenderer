cmake_minimum_required(VERSION 3.18)
project(Renderer C)

set(LLVM_VERSION 11.0.1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Werror)
add_compile_options(-pedantic )

add_executable(renderer
  src/main.c
  src/camera.c
  src/clipping.c
  src/display.c
  src/light.c
  src/matrix.c
  src/mesh.c
  src/swap.c
  src/texture.c
  src/triangle.c
  src/upng.c
  src/vector.c)

target_link_libraries(renderer m SDL2)
