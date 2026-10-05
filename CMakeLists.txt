cmake_minimum_required(VERSION 3.10)
project(BearEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IMGUI_DIR ${ROOT_DIR}/exports/imgui-master)
set(GLAD_DIR ${ROOT_DIR}/exports/glad)
set(STB_DIR ${ROOT_DIR}/exports/stb)
set(GLM_DIR ${ROOT_DIR}/exports/glm)

add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui_lib STATIC ${IMGUI_SOURCES})

set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)

set_source_files_properties(
    ${IMGUI_SOURCES}
    PROPERTIES
    LANGUAGE CXX
    COMPILE_FLAGS "-x c++"
)

target_include_directories(imgui_lib PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLAD_DIR}/include
)

target_compile_definitions(imgui_lib PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
)

target_compile_options(imgui_lib PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp
    "#define STB_IMAGE_IMPLEMENTATION\n"
    "#include \"${STB_DIR}/stb_image.h\"\n"
)

add_library(stb_lib STATIC ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp)
target_include_directories(stb_lib PUBLIC ${STB_DIR})
set_target_properties(stb_lib PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${ROOT_DIR}/src/core/*.cpp")

#FUCK EDITOR
file(GLOB_RECURSE EDITOR_SOURCES_TO_REMOVE 
    "${ROOT_DIR}/src/core/users_interface/Editor/*.cpp"
)
list(REMOVE_ITEM CORE_SOURCES ${EDITOR_SOURCES_TO_REMOVE})
#END FUCKING

file(GLOB_RECURSE RHI_SOURCES_TO_REMOVE 
    "${ROOT_DIR}/src/core/graphics_engine/RHI/*.cpp"
)
list(REMOVE_ITEM CORE_SOURCES ${RHI_SOURCES_TO_REMOVE})

add_executable(game ${ROOT_DIR}/main.cpp ${CORE_SOURCES})

target_include_directories(game PRIVATE
    ${ROOT_DIR}/include

    ${ROOT_DIR}/include/Math
    ${ROOT_DIR}/include/Math/Algebra
    ${ROOT_DIR}/include/Math/Algebra/Matrix
    ${ROOT_DIR}/include/Math/Algebra/Transform
    ${ROOT_DIR}/include/Math/Algebra/Vector
    ${ROOT_DIR}/include/Math/Geometry

    ${ROOT_DIR}/include/Audio
    ${ROOT_DIR}/include/Audio/Resources

    ${ROOT_DIR}/include/Graphics
    ${ROOT_DIR}/include/Graphics/core
    ${ROOT_DIR}/include/Graphics/core/base_graphics
    ${ROOT_DIR}/include/Graphics/resources

    ${ROOT_DIR}/include/Graphics/2D
    ${ROOT_DIR}/include/Graphics/2D/features
    ${ROOT_DIR}/include/Graphics/2D/renderers
    ${ROOT_DIR}/include/Graphics/2D/Shaders
    ${ROOT_DIR}/include/Graphics/2D/sprite

    ${ROOT_DIR}/include/Graphics/3D
    ${ROOT_DIR}/include/Graphics/3D/lights
    ${ROOT_DIR}/include/Graphics/3D/models
    ${ROOT_DIR}/include/Graphics/3D/models/model_features
    ${ROOT_DIR}/include/Graphics/3D/models/model_renderers
    ${ROOT_DIR}/include/Graphics/3D/shaders
    ${ROOT_DIR}/include/Graphics/3D/shaders/quad
    ${ROOT_DIR}/include/Graphics/3D/shaders/simple
    ${ROOT_DIR}/include/Graphics/3D/shaders/transparent
    ${ROOT_DIR}/include/Graphics/3D/uv

    ${ROOT_DIR}/include/Graphics/RHI
    ${ROOT_DIR}/include/Graphics/RHI/Base
    ${ROOT_DIR}/include/Graphics/RHI/Base/Core
    ${ROOT_DIR}/include/Graphics/RHI/Base/Executors
    ${ROOT_DIR}/include/Graphics/RHI/Base/Models
    ${ROOT_DIR}/include/Graphics/RHI/Fabric
    ${ROOT_DIR}/include/Graphics/RHI/OpenGL
    ${ROOT_DIR}/include/Graphics/RHI/OpenGL/Core
    ${ROOT_DIR}/include/Graphics/RHI/OpenGL/Executors
    ${ROOT_DIR}/include/Graphics/RHI/Vulkan

    ${ROOT_DIR}/include/Physics
    ${ROOT_DIR}/include/Physics/Body
    ${ROOT_DIR}/include/Physics/Body/Electromagnetic
    ${ROOT_DIR}/include/Physics/Body/Fluid
    ${ROOT_DIR}/include/Physics/Body/Mechanical
    ${ROOT_DIR}/include/Physics/Body/Mechanical/Elasticity
    ${ROOT_DIR}/include/Physics/Body/PhysicCollisions
    ${ROOT_DIR}/include/Physics/Body/Specialized
    ${ROOT_DIR}/include/Physics/Body/Thermal
    ${ROOT_DIR}/include/Physics/Colliders
    ${ROOT_DIR}/include/Physics/Colliders/2D
    ${ROOT_DIR}/include/Physics/Colliders/3D
    ${ROOT_DIR}/include/Physics/Colliders/Base
    ${ROOT_DIR}/include/Physics/Events
    ${ROOT_DIR}/include/Physics/Events/Fluid
    ${ROOT_DIR}/include/Physics/Events/Mechanical
    ${ROOT_DIR}/include/Physics/World

    ${ROOT_DIR}/include/Shared/Input
    ${ROOT_DIR}/include/Shared/Resource
    ${ROOT_DIR}/include/Shared/Resource/ResourcesTypes
    ${ROOT_DIR}/include/Shared/Time

    ${ROOT_DIR}/include/Systems/Hierarchy
    ${ROOT_DIR}/include/Systems/Tags

    ${ROOT_DIR}/include/UI
    ${ROOT_DIR}/include/UI/Resources
    ${ROOT_DIR}/include/UI/ui_elements
    ${ROOT_DIR}/include/UI/ui_rendering
    ${ROOT_DIR}/include/UI/ui_shaders

    ${ROOT_DIR}/include/World
    ${ROOT_DIR}/include/World/Components
    ${ROOT_DIR}/include/World/Scene
    ${ROOT_DIR}/include/World/Serialization
    ${ROOT_DIR}/include/World/Serialization/SceneParser

    ${GLM_DIR}
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${FREETYPE_INCLUDE_DIRS}
)


file(GLOB_RECURSE EDITOR_SOURCES "${ROOT_DIR}/include/core/users_interface/Editor/*.h")

foreach(SOURCE_FILE ${EDITOR_SOURCES} ${EDITOR_UTILS_SOURCES})
    get_filename_component(SOURCE_DIR ${SOURCE_FILE} DIRECTORY)
    list(APPEND EDITOR_INCLUDE_DIRS ${SOURCE_DIR})
endforeach()

list(REMOVE_DUPLICATES EDITOR_INCLUDE_DIRS)

foreach(EDITOR_INCLUDE_DIR ${EDITOR_INCLUDE_DIRS})
    target_include_directories(game PRIVATE ${EDITOR_INCLUDE_DIR})
endforeach()


target_compile_definitions(game PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
    STB_IMAGE_STATIC
    STBI_MAX_DIMENSIONS=4096
)

target_compile_options(game PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

target_link_libraries(game PRIVATE
    glad_lib
    imgui_lib
    stb_lib
    OpenGL::GL
    glfw
    SDL2::SDL2
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
    ${FREETYPE_LIBRARIES}
)

message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Core files: ${CORE_SOURCES}")
message(STATUS "GLM: ${GLM_DIR}")
message(STATUS "ImGui language: C++ (forced)")
message(STATUS "========================================")