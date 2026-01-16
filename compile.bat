@echo off
echo ===============================
echo  Bear Engine - Final Compile
echo ===============================

rem 1. Все исходники
set SOURCES=main.cpp src/core/phisic_engine/BoxCollider2D.cpp src/dependencies/glad/src/glad.c src/core/phisic_engine/Rigidbody2D.cpp src/core/GameObject.cpp src/core/graphics_engine/Render.cpp src\core\graphics_engine\DrawingWindow.cpp src\core\math\Transform2D.cpp src\core\math\Vector2.cpp

rem 2. Все пути
set INCLUDE=^
 -I.^
 -Isrc^
 -Isrc/core^
 -Isrc/core/math^
 -Isrc/core/graphics_engine^
 -Isrc/core/phisic_engine^
 -Iinclude^
 -Iinclude/core/phisic_engine^
 -Isrc/dependencies/glad/include^
 -Isrc/dependencies/GLFW/include

rem 3. Библиотеки
set LIBS=^
 -Lsrc/dependencies/GLFW/lib-mingw-w64^
 -lglfw3^
 -lopengl32^
 -lgdi32^
 -static-libgcc^
 -static-libstdc++

rem 4. Компилируем ВСЕ файлы
echo Компилируем: %SOURCES%
echo.
g++ %SOURCES% -o game.exe %INCLUDE% %LIBS% -std=c++17

if %errorlevel% equ 0 (
    echo.
    echo ✅ SUCCESS! Physics engine готов!
    echo.
    game.exe
) else (
    echo.
    echo ❌ COMPILATION FAILED
    echo Убедись что:
    echo 1. BoxCollider2D.cpp существует
    echo 2. Пути к файлам верные
    echo 3. GLFW библиотеки на месте
)

pause