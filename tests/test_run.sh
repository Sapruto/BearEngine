RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  ЗАПУСК ТЕСТОВ ДВИЖКА${NC}"
echo -e "${BLUE}========================================${NC}"

if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Ошибка: g++ не найден. Установите компилятор.${NC}"
    exit 1
fi

echo -e "\n${YELLOW}Сборка тестов...${NC}"

g++ -std=c++17 -O2 \
    -I./include \
    math/test_math.cpp \
    -o TestCore \
    -lm

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции тестов!${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Сборка завершена${NC}"

echo -e "\n${YELLOW}Запуск тестов...${NC}"
./TestCore
TEST_RESULT=$?

rm -f TestCore

if [ $TEST_RESULT -eq 0 ]; then
    echo -e "\n${GREEN}Все тесты пройдены!${NC}"
    exit 0
else
    echo -e "\n${RED}Некоторые тесты провалены!${NC}"
    exit 1
fi