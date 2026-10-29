cmake_minimum_required(VERSION 3.10)
project(CubeExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
find_package(assimp REQUIRED)
find_package(SDL2 REQUIRED)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(GLAD_DIR ${ROOT_DIR}/exports/glad)
set(GLM_DIR  ${ROOT_DIR}/exports/glm)
set(STB_DIR  ${ROOT_DIR}/exports/stb)

add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

file(GLOB_RECURSE RHI_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/RHI/*.cpp"
)

file(GLOB_RECURSE WORLD_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/World/*.cpp"
)

file(GLOB_RECURSE MATH_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/Math/*.cpp"
)

file(GLOB_RECURSE SYSTEMS_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/Systems/*.cpp"
)

set(DOMAINS_SOURCES
    "${ROOT_DIR}/src/Domains/Renderer/Base/GraphicInstance.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/Base/Shader.cpp"

    "${ROOT_DIR}/src/Domains/Renderer/3D/Scene/RendererScene3D.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/3D/Passes/DefaultPass.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/3D/Mesh/Mesh.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/3D/Mesh/Renderable3D.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/3D/Settings/Cameras/Camera3D.cpp"
    "${ROOT_DIR}/src/Domains/Renderer/3D/Settings/Lights/DirectionalLight3D.cpp"
)

set(ALL_SOURCES
    ${RHI_SOURCES}
    ${DOMAINS_SOURCES}
    ${WORLD_SOURCES}
    ${MATH_SOURCES}
    ${SYSTEMS_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/example1.cpp"
)

add_executable(cube_example ${ALL_SOURCES})

target_include_directories(cube_example PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/include

    ${ROOT_DIR}/src/Systems
    ${ROOT_DIR}/include/Systems

    ${ROOT_DIR}/include/Math
    ${ROOT_DIR}/include/Math/Vector
    ${ROOT_DIR}/include/Math/Transform
    ${ROOT_DIR}/include/Math/Matrix

    ${GLM_DIR}
    ${GLAD_DIR}/include
    ${STB_DIR}

    ${SDL2_INCLUDE_DIRS}
)

target_compile_definitions(cube_example PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
    RHI_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
    PROJECT_ROOT_DIR="${ROOT_DIR}"
)

target_compile_options(cube_example PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

target_link_libraries(cube_example PRIVATE
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
    assimp::assimp
    ${SDL2_LIBRARIES}
)