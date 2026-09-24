cmake_minimum_required(VERSION 3.10)
project(RHITriangle)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

set(ROOT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(GLAD_DIR  ${ROOT_DIR}/src/dependencies/glad)
set(GLM_DIR   ${ROOT_DIR}/src/dependencies/glm)
set(STB_DIR   ${ROOT_DIR}/src/dependencies/stb)

add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

file(GLOB_RECURSE RHI_SOURCES
    "${ROOT_DIR}/src/core/graphics_engine/RHI/*.cpp"
)
list(FILTER RHI_SOURCES EXCLUDE REGEX "FabricRHI\\.cpp$")

list(APPEND RHI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Examples/OpenGLTriangle.cpp"
)

add_executable(rhi_triangle ${RHI_SOURCES})

target_include_directories(rhi_triangle PRIVATE
    ${ROOT_DIR}/src/core/graphics_engine
    ${ROOT_DIR}/include/core/graphics_engine

    ${ROOT_DIR}/src
    ${ROOT_DIR}/include

    ${ROOT_DIR}/include/core/math
    ${ROOT_DIR}/include/core/math/Vector
    ${ROOT_DIR}/include/core/math/Transform

    ${GLM_DIR}
    ${GLAD_DIR}/include
    ${STB_DIR}
)

target_compile_definitions(rhi_triangle PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
    RHI_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
)

target_compile_options(rhi_triangle PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

target_link_libraries(rhi_triangle PRIVATE
    glad_lib
    OpenGL::GL
    glfw
    Threads::Threads
    pthread
    dl
    m
    X11
    xcb
    Xrandr
    Xinerama
    Xi
    Xcursor
    Xext
)

add_custom_command(TARGET rhi_triangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:rhi_triangle>/shaders
)