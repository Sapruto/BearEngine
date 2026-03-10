GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "========================================"
echo "   BearEngine - Сборка проекта"
echo "========================================"
echo ""

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}[ERROR] CMake не найден!${NC}"
    echo "Установите: sudo apt install cmake"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo -e "${RED}[ERROR] Make не найден!${NC}"
    echo "Установите: sudo apt install build-essential"
    exit 1
fi

mkdir build
cd build

echo -e "${GREEN}[1/2] Генерация...${NC}"
cmake .. > build_log.txt 2>&1

if [ $? -ne 0 ]; then
    echo ""
    echo -e "${RED}[ERROR] Ошибка генерации!${NC}"
    echo ""
    grep -i "error" build_log.txt | head -20
    echo ""
    echo "========================================"
    echo "Смотри CMakeLists.txt - проблема с зависимостями"
    echo "========================================"
    exit 1
fi

echo -e "${GREEN}[2/2] Компиляция...${NC}"
make -j$(nproc) > compile_log.txt 2>&1

if [ $? -ne 0 ]; then
    echo ""
    echo -e "${RED}[ERROR] Ошибка компиляции!${NC}"
    echo ""
    grep -i "error" compile_log.txt | head -20
    echo ""
    exit 1
fi

cd ..

echo ""
echo "========================================"
echo -e "${GREEN}   ГОТОВО!${NC}"
echo "========================================"
echo ""

EXE_PATH=$(find build -type f -executable -name "game" | head -1)

if [ -n "$EXE_PATH" ]; then
    echo "Программа: $EXE_PATH"
    echo ""
    echo "Запустить? (y/n)"
    read -r answer
    if [[ "$answer" =~ ^[Yy]$ ]]; then
        ./$EXE_PATH
    fi
else
    echo -e "${RED}Исполняемый файл не найден :(${NC}"
fi

echo ""
read -p "Нажмите Enter для продолжения..."
