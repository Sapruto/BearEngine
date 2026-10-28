cmake_minimum_required(VERSION 3.20)
project(SceneSerializationTests C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(GLAD_DIR ${ROOT_DIR}/exports/glad)
set(STB_DIR  ${ROOT_DIR}/exports/stb)
set(IMGUI_DIR ${ROOT_DIR}/exports/imgui-master)

find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE WORLD_SOURCES CONFIGURE_DEPENDS
    ${ROOT_DIR}/src/World/*.cpp
)
file(GLOB_RECURSE SYSTEM_SOURCES CONFIGURE_DEPENDS
    ${ROOT_DIR}/src/Systems/*.cpp
)
file(GLOB_RECURSE HIERARCHY_SOURCES CONFIGURE_DEPENDS
    ${ROOT_DIR}/src/Systems/Hierarchy/*.cpp
)
file(GLOB_RECURSE LOGGER_SOURCES CONFIGURE_DEPENDS
    ${ROOT_DIR}/src/Logger/*.cpp
)

add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp
    "#define STB_IMAGE_IMPLEMENTATION\n"
    "#include \"${STB_DIR}/stb_image.h\"\n"
)
add_library(stb_lib STATIC ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp)
target_include_directories(stb_lib PUBLIC ${STB_DIR})
set_target_properties(stb_lib PROPERTIES LINKER_LANGUAGE CXX)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${WORLD_SOURCES}
    ${SYSTEM_SOURCES}
    ${HIERARCHY_SOURCES}
    ${LOGGER_SOURCES}
)

add_executable(scene_tests ${TEST_SOURCES})

target_include_directories(scene_tests PRIVATE
    ${ROOT_DIR}/include

    ${ROOT_DIR}/include/Math
    ${ROOT_DIR}/include/Math/Vector
    ${ROOT_DIR}/include/Math/Matrix
    ${ROOT_DIR}/include/Math/Transform

    ${ROOT_DIR}/include/World
    ${ROOT_DIR}/include/World/Scene
    ${ROOT_DIR}/include/World/Components
    ${ROOT_DIR}/include/World/Serialization
    ${ROOT_DIR}/include/World/Serialization/SceneParser
    ${ROOT_DIR}/include/World/Serialization/SceneParser/Chunks

    ${ROOT_DIR}/include/Systems
    ${ROOT_DIR}/include/Systems/Hierarchy

    ${ROOT_DIR}/include/Systems
    ${ROOT_DIR}/include/Systems/Resource
    ${ROOT_DIR}/include/Systems/Resource/ResourcesTypes
    ${ROOT_DIR}/include/Systems/Logger
    ${ROOT_DIR}/include/Systems/Input
    ${ROOT_DIR}/include/Systems/Time

    ${ROOT_DIR}/include/Renderer

    ${GLAD_DIR}/include
    ${STB_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_BINARY_DIR}
    ${FREETYPE_INCLUDE_DIRS}
)

target_compile_options(scene_tests PRIVATE -Wall -Wextra -g -O0)

target_link_libraries(scene_tests PRIVATE
    glad_lib
    stb_lib
    OpenGL::GL
    glfw
    SDL2::SDL2
    assimp::assimp
    dl
    m
    pthread
    ${FREETYPE_LIBRARIES}
)

enable_testing()
add_test(NAME scene_round_trip COMMAND scene_tests)