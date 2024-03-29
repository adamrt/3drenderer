cmake_minimum_required(VERSION 3.18)
project(Renderer CXX)

set(LLVM_VERSION 11.0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Werror)
add_compile_options(-pedantic)

add_executable(renderer
  src/main.cpp
  src/Camera.cpp
  src/clipping.cpp
  src/Engine.cpp
  src/Framebuffer.cpp
  src/Light.cpp
  src/mesh.cpp
  src/Window.cpp
  src/texture.cpp
  src/upng.cpp)


target_link_libraries(renderer m SDL2)
