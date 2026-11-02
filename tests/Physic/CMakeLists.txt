cmake_minimum_required(VERSION 3.10)
project(CubePhysicsExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
find_package(assimp REQUIRED)
find_package(SDL2 REQUIRED)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(GLAD_DIR ${ROOT_DIR}/exports/glad)
set(GLM_DIR  ${ROOT_DIR}/exports/glm)
set(STB_DIR  ${ROOT_DIR}/exports/stb)

add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

file(GLOB_RECURSE RHI_SOURCES      CONFIGURE_DEPENDS "${ROOT_DIR}/src/RHI/*.cpp")
file(GLOB_RECURSE WORLD_SOURCES    CONFIGURE_DEPENDS "${ROOT_DIR}/src/World/*.cpp")
file(GLOB_RECURSE MATH_SOURCES     CONFIGURE_DEPENDS "${ROOT_DIR}/src/Math/*.cpp")
file(GLOB_RECURSE SYSTEMS_SOURCES  CONFIGURE_DEPENDS "${ROOT_DIR}/src/Systems/*.cpp")

set(DOMAIN_SOURCES "")

file(GLOB_RECURSE _renderer_base CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/Domains/Renderer/Base/*.cpp")
list(APPEND DOMAIN_SOURCES ${_renderer_base})

file(GLOB_RECURSE _renderer_3d CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/Domains/Renderer/3D/*.cpp")
list(APPEND DOMAIN_SOURCES ${_renderer_3d})

file(GLOB_RECURSE _renderer_3d_inc CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/Domains/Renderer/3D/*.cpp")
list(APPEND DOMAIN_SOURCES ${_renderer_3d_inc})

file(GLOB_RECURSE _physics CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/Domains/Physics/*.cpp")
list(APPEND DOMAIN_SOURCES ${_physics})

file(GLOB_RECURSE _physics_inc CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/Domains/Physics/*.cpp")
list(APPEND DOMAIN_SOURCES ${_physics_inc})

foreach(_f
    "${ROOT_DIR}/src/Domains/Collisions/Core/BaseCollider.cpp"
    "${ROOT_DIR}/src/Domains/Collisions/Core/CollisionReaction.cpp"
    "${ROOT_DIR}/src/Domains/Collisions/Core/ColliderManager.cpp"
    "${ROOT_DIR}/include/Domains/Collisions/Core/BaseCollider.cpp"
    "${ROOT_DIR}/include/Domains/Collisions/Core/CollisionReaction.cpp"
    "${ROOT_DIR}/include/Domains/Collisions/Core/ColliderManager.cpp"
    "${ROOT_DIR}/src/Domains/Physics/Body/PhysicalBody.cpp"
    "${ROOT_DIR}/include/Domains/Physics/Body/PhysicalBody.cpp"
)
    if(EXISTS "${_f}")
        list(APPEND DOMAIN_SOURCES "${_f}")
    endif()
endforeach()

list(FILTER DOMAIN_SOURCES EXCLUDE REGEX "/TransparencyRenderer\\.cpp$")

set(ALL_SOURCES
    ${RHI_SOURCES}
    ${WORLD_SOURCES}
    ${MATH_SOURCES}
    ${SYSTEMS_SOURCES}
    ${DOMAIN_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/cube_physics_example.cpp"
)

list(FILTER ALL_SOURCES EXCLUDE REGEX "/TransparencyRenderer\\.")

add_executable(cube_physics_example ${ALL_SOURCES})

target_include_directories(cube_physics_example PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/include

    ${ROOT_DIR}/src/Systems
    ${ROOT_DIR}/include/Systems

    ${ROOT_DIR}/include/Math
    ${ROOT_DIR}/include/Math/Vector
    ${ROOT_DIR}/include/Math/Transform
    ${ROOT_DIR}/include/Math/Matrix

    ${ROOT_DIR}/src/Domains
    ${ROOT_DIR}/include/Domains

    ${ROOT_DIR}/src/Domains/Core
    ${ROOT_DIR}/include/Domains/Core

    ${ROOT_DIR}/src/Domains/Physics
    ${ROOT_DIR}/include/Domains/Physics

    ${ROOT_DIR}/src/Domains/Physics/Body
    ${ROOT_DIR}/include/Domains/Physics/Body

    ${ROOT_DIR}/src/Domains/Physics/Core
    ${ROOT_DIR}/include/Domains/Physics/Core

    ${ROOT_DIR}/src/Domains/Physics/Simulations
    ${ROOT_DIR}/include/Domains/Physics/Simulations

    ${ROOT_DIR}/src/Domains/Collisions
    ${ROOT_DIR}/include/Domains/Collisions

    ${ROOT_DIR}/src/Domains/Collisions/Core
    ${ROOT_DIR}/include/Domains/Collisions/Core

    ${ROOT_DIR}/src/Domains/Collisions/2D
    ${ROOT_DIR}/include/Domains/Collisions/2D

    ${ROOT_DIR}/src/Domains/Collisions/3D
    ${ROOT_DIR}/include/Domains/Collisions/3D

    ${ROOT_DIR}/src/Domains/Renderer
    ${ROOT_DIR}/include/Domains/Renderer

    ${ROOT_DIR}/src/Domains/Renderer/Base
    ${ROOT_DIR}/include/Domains/Renderer/Base

    ${ROOT_DIR}/src/Domains/Renderer/3D
    ${ROOT_DIR}/include/Domains/Renderer/3D

    ${ROOT_DIR}/src/Domains/Renderer/3D/Scene
    ${ROOT_DIR}/include/Domains/Renderer/3D/Scene

    ${ROOT_DIR}/src/Domains/Renderer/3D/Passes
    ${ROOT_DIR}/include/Domains/Renderer/3D/Passes

    ${ROOT_DIR}/src/Domains/Renderer/3D/Mesh
    ${ROOT_DIR}/include/Domains/Renderer/3D/Mesh

    ${ROOT_DIR}/src/Domains/Renderer/3D/Settings
    ${ROOT_DIR}/include/Domains/Renderer/3D/Settings

    ${ROOT_DIR}/src/Domains/Renderer/3D/Settings/Cameras
    ${ROOT_DIR}/include/Domains/Renderer/3D/Settings/Cameras

    ${ROOT_DIR}/src/Domains/Renderer/3D/Settings/Lights
    ${ROOT_DIR}/include/Domains/Renderer/3D/Settings/Lights

    ${ROOT_DIR}/src/Domains/Renderer/3D/Materials
    ${ROOT_DIR}/include/Domains/Renderer/3D/Materials

    ${GLM_DIR}
    ${GLAD_DIR}/include
    ${STB_DIR}

    ${SDL2_INCLUDE_DIRS}
)

target_compile_definitions(cube_physics_example PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
    RHI_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
    PROJECT_ROOT_DIR="${ROOT_DIR}"
)

target_compile_options(cube_physics_example PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

target_link_libraries(cube_physics_example PRIVATE
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