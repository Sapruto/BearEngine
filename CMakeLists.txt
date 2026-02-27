cmake_minimum_required(VERSION 3.10)
project(TriangleChaos)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/graphics_engine
    ${CMAKE_CURRENT_SOURCE_DIR}/math
)

include_directories(third_party/glm)
add_executable(game main.cpp dependencies/glad/src/glad.c)

target_link_libraries(game glfw opengl32 gdi32)
