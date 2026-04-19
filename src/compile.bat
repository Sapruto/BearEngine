@echo off
setlocal enabledelayedexpansion

echo Устанавливаем пути...
set INCLUDE_PATHS=-I. -Icore -Icore/graphics_engine -Idependencies/glad/include -Idependencies/glfw/include
set LIB_PATHS=-Ldependencies/glfw/lib-mingw-w64
set LIBS=-lglfw3 -lopengl32 -lgdi32 -static-libgcc -static-libstdc++

echo Компилируем...
g++ main.cpp dependencies/glad/src/glad.c -o game.exe !INCLUDE_PATHS! !LIB_PATHS! !LIBS!

if %errorlevel% equ 0 (
    echo ✅ Успешно скомпилировано!
    echo Запускаю игру...
    echo.
    game.exe
) else (
    echo ❌ Ошибка компиляции
    echo Проверьте пути к файлам
)

pause