cmake_minimum_required(VERSION 3.10)
project(BearEngine)

# ========== БАЗОВЫЕ НАСТРОЙКИ ==========
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========== РЕЖИМ СБОРКИ ==========
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ========== ПОИСК БИБЛИОТЕК ==========
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

# ========== ПУТИ ==========
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IMGUI_DIR ${ROOT_DIR}/src/dependencies/imgui-master)
set(GLAD_DIR ${ROOT_DIR}/src/dependencies/glad)
set(STB_DIR ${ROOT_DIR}/src/dependencies/stb)
set(GLM_DIR ${ROOT_DIR}/src/dependencies/glm)

# ========== СОЗДАЕМ БИБЛИОТЕКИ ==========

# GLAD библиотека (C)
add_library(glad_lib STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${GLAD_DIR}/include)
set_target_properties(glad_lib PROPERTIES LINKER_LANGUAGE C)

# ImGui библиотека (C++ - ПРИНУДИТЕЛЬНО)
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

# ЯВНО указываем язык C++ для всей библиотеки
set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)

# Для каждого файла явно указываем язык C++
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

# STB библиотека (C)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp
    "#define STB_IMAGE_IMPLEMENTATION\n"
    "#include \"${STB_DIR}/stb_image.h\"\n"
)

add_library(stb_lib STATIC ${CMAKE_CURRENT_BINARY_DIR}/stb_impl.cpp)
target_include_directories(stb_lib PUBLIC ${STB_DIR})
set_target_properties(stb_lib PROPERTIES LINKER_LANGUAGE CXX)

# ========== СОБИРАЕМ ФАЙЛЫ ДВИЖКА ==========
file(GLOB_RECURSE CORE_SOURCES "${ROOT_DIR}/src/core/*.cpp")

# ========== ГЛАВНЫЙ ИСПОЛНЯЕМЫЙ ФАЙЛ ==========
add_executable(game ${ROOT_DIR}/main.cpp ${CORE_SOURCES})

# ========== ИНКЛУДЫ ==========
target_include_directories(game PRIVATE
    # Проект
    ${ROOT_DIR}/include
    ${ROOT_DIR}/src
    
    # Core папки
    ${ROOT_DIR}/include/core
    ${ROOT_DIR}/include/core/audio_engine
    ${ROOT_DIR}/include/core/exe_compiler
    ${ROOT_DIR}/include/core/users_interface
    ${ROOT_DIR}/include/core/user_settings
    ${ROOT_DIR}/include/core/graphics_engine
    ${ROOT_DIR}/include/core/graphics_engine/BaseGraphic
    ${ROOT_DIR}/include/core/graphics_engine/Shaders
    ${ROOT_DIR}/include/core/graphics_engine/Shaders/SpriteShaders
    ${ROOT_DIR}/include/core/graphics_engine/Sprite
    ${ROOT_DIR}/include/core/system_engine
    ${ROOT_DIR}/include/core/system_engine/atributs
    ${ROOT_DIR}/include/core/system_engine/component_system
    ${ROOT_DIR}/include/core/system_engine/input_system
    ${ROOT_DIR}/include/core/system_engine/resource_system
    ${ROOT_DIR}/include/core/system_engine/resource_system/ResourcesTypes
    ${ROOT_DIR}/include/core/system_engine/tags_system
    ${ROOT_DIR}/include/core/system_engine/time_system
    ${ROOT_DIR}/include/core/math
    ${ROOT_DIR}/include/core/math/2D
    ${ROOT_DIR}/include/core/math/3D
    ${ROOT_DIR}/include/core/scene_engine
    ${ROOT_DIR}/include/core/scene_engine/SceneToFile
    ${ROOT_DIR}/include/core/geometry
    ${ROOT_DIR}/include/core/geometry/colliders_manager
    ${ROOT_DIR}/include/core/geometry/colliders
    ${ROOT_DIR}/include/core/geometry/colliders/2D
    ${ROOT_DIR}/include/core/geometry/colliders/3D
    ${ROOT_DIR}/include/core/phisic_engine
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Electromagnetic
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Fluid
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Mechanical
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Specialized
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Thermal
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicsEvents
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicsEvents/Mechanical
    ${ROOT_DIR}/include/core/phisic_engine/new_physiscs/PhysicsManagers
    
    # Зависимости
    ${GLM_DIR}
    ${GLM_DIR}/GLM
    ${GLM_DIR}/GLM/simd
    ${GLM_DIR}/GLM/gtx
    ${GLM_DIR}/GLM/gtc
    ${GLM_DIR}/GLM/detail
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# ========== ДЕФАЙНЫ ==========
target_compile_definitions(game PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
    STB_IMAGE_STATIC
    STBI_MAX_DIMENSIONS=4096
)

# ========== ФЛАГИ ==========
target_compile_options(game PRIVATE
    -Wall -Wextra -g -O0
    -fPIC
)

# ========== ЛИНКОВКА ==========
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
)

# ========== ВЫВОД ИНФОРМАЦИИ ==========
message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Core files: ${CORE_SOURCES}")
message(STATUS "GLM: ${GLM_DIR}")
message(STATUS "ImGui language: C++ (forced)")
message(STATUS "========================================")