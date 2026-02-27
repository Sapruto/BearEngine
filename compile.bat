@echo off
echo ===============================
echo  FINAL BUILD - SDL2 + Windows
echo ===============================

set SDL_PATH=src\dependencies\SDL2-2.28.5\x86_64-w64-mingw32

set SOURCES=main.cpp src\core\math\geometry\colliders\BoxCollider2D.cpp src/dependencies/glad/src/glad.c src/core/phisic_engine/Rigidbody2D.cpp src/core/GameObject.cpp src\core\graphics_engine\DrawingWindow.cpp src\core\math\Transform2D.cpp src\core\math\Vector2.cpp src\core\system_engine\input_system\InputSystem.cpp src\core\system_engine\time_system\Time.cpp src\core\scene_engine\Scene.cpp src\core\scene_engine\SceneManager.cpp src\core\math\geometry\colliders\Polygon2D.cpp src\core\math\geometry\InserectionSegments2D.cpp src\core\system_engine\tags_system\Tag.cpp src\core\system_engine\tags_system\TagManager.cpp src\core\system_engine\resource_system\ResourceManager.cpp src\core\system_engine\resource_system\ResourceFactory.cpp src\core\system_engine\resource_system\ResourcesTypes\Texture.cpp src\core\system_engine\resource_system\ResourcesTypes\Audio.cpp src\core\system_engine\resource_system\ResourcesTypes\Font.cpp src\core\graphics_engine\Sprite\Sprite.cpp src\core\graphics_engine\Sprite\SpriteRenderer.cpp src\core\scene_engine\SceneToFile\SceneCreator.cpp src\core\scene_engine\SceneToFile\SceneDeserializer.cpp src\core\scene_engine\SceneToFile\SceneSerializer.cpp src\core\math\geometry\colliders_manager\ColliderManager.cpp src\core\math\geometry\colliders\Collider.cpp src\core\graphics_engine\Shader.cpp src\core\graphics_engine\GraphicsManager.cpp src\core\graphics_engine\Camera.cpp

set IMGUI_SOURCES=src\dependencies\imgui-master\imgui.cpp src\dependencies\imgui-master\imgui_draw.cpp src\dependencies\imgui-master\imgui_tables.cpp src\dependencies\imgui-master\imgui_widgets.cpp src\dependencies\imgui-master\backends\imgui_impl_glfw.cpp src\dependencies\imgui-master\backends\imgui_impl_opengl3.cpp

set INCLUDE=-I. -Isrc -Isrc/core -Isrc/core/math -Isrc/core/graphics_engine -Isrc/core/phisic_engine -Iinclude -Iinclude/core/phisic_engine -Isrc/dependencies/glad/include -Isrc/dependencies/GLFW/include -I%SDL_PATH%/include -Isrc/dependencies/imgui-master -Isrc/dependencies/imgui-master/backends

echo Компиляция с консолью для отладки...
g++ %SOURCES% %IMGUI_SOURCES% -o game.exe %INCLUDE% -Lsrc/dependencies/GLFW/lib-mingw-w64 -L%SDL_PATH%/lib -lglfw3 -lopengl32 -lgdi32 -lSDL2main -lSDL2 -std=c++17 -lstdc++fs -static-libgcc -static-libstdc++ -Wl,-subsystem,console -g -O0

if %errorlevel% equ 0 (
    echo КОМПИЛЯЦИЯ УСПЕШНА!
    copy "%SDL_PATH%\bin\SDL2.dll" .
    
    echo.
    echo ===============================
    echo  ЗАПУСК С КОНСОЛЬЮ
    echo ===============================
    echo.
    
    start cmd /k "echo === ОТЛАДОЧНАЯ КОНСОЛЬ === && echo. && game.exe && echo. && echo Программа завершена. Код: %errorlevel% && echo Нажмите любую клавишу для закрытия консоли... && pause > nul"
    
    echo Консоль запущена отдельно.
) else (
    echo КОМПИЛЯЦИЯ ПРОВАЛИЛАСЬ!
    pause
)