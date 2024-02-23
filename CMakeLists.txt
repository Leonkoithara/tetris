cmake_minimum_required(VERSION 3.16.3)

project(tetris)

set(CMAKE_BUILD_TYPE Debug)

set(CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(out src/main.cpp)
target_link_libraries(
  out
  ${SDL2_LIBRARIES}
  SDL2_image
  SDL2_ttf
)
