#include "level.h"
#include "player.h"
#include <sstream>
#include <iostream>

extern Player player;  // Add extern declaration for the Player instance

// конструктор класса level - инициализирует начальные значения
Level::Level() : rows(0), columns(0), levelData(nullptr), currentLevelIndex(0) {}

// деструктор - освобождает память при удалении объекта
Level::~Level() {
    unloadLevel();
}

// проверяет, находится ли клетка внутри границ уровня
bool Level::isInsideLevel(int row, int column) const {
    if (row < 0 || row >= rows) return false;
    if (column < 0 || column >= columns) return false;
    return true;
}

// проверяет столкновение сущности с указанным типом клетки
bool Level::isColliding(Vector2 pos, char lookFor) const {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс сущности для проверки столкновений

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            if (!isInsideLevel(row, column)) continue;
            // пропускаем если координаты вне уровня

            if (getLevelCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float)column, (float)row, 1.0f, 1.0f};
                // создаём хитбокс блока и проверяем пересечение

                if (CheckCollisionRecs(entityHitbox, blockHitbox)) {
                    return true;
                    // если произошло столкновение — true
                }
            }
        }
    }
    return false;
    // если не было найдено столкновений — false
}

// получает ссылку на клетку, с которой произошло столкновение
char& Level::getCollider(Vector2 pos, char lookFor) {
    Rectangle playerHitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс игрока

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            if (!isInsideLevel(row, column)) continue;
            // пропускаем клетки за пределами уровня

            if (getLevelCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float)column, (float)row, 1.0f, 1.0f};
                // создаём хитбокс блока

                if (CheckCollisionRecs(playerHitbox, blockHitbox)) {
                    return getLevelCell(row, column);
                    // возвращаем ссылку на столкнувшуюся клетку
                }
            }
        }
    }

    return getLevelCell(pos.x, pos.y);
    // если ничего не найдено — возвращаем ссылку на текущую клетку
}

// сбрасывает индекс текущего уровня
void Level::resetLevelIndex() {
    currentLevelIndex = 0;
}

// загружает уровень с указанным смещением
void Level::loadLevel(int offset) {
    currentLevelIndex += offset;
    // обновляем индекс уровня с учётом смещения

    if (currentLevelIndex >= LEVEL_COUNT) {
        game_state = VICTORY_STATE;
        create_victory_menu_background();
        currentLevelIndex = 0;
        return;
        // если уровни закончились — переходим к экрану победы
    }

    // копируем данные уровня
    rows = LEVELS[currentLevelIndex].rows;
    columns = LEVELS[currentLevelIndex].columns;
    levelData = new char[rows * columns];

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            levelData[row * columns + column] = LEVELS[currentLevelIndex].data[row * columns + column];
        }
    }

    player.spawn(*this);  // Use the Player class method instead of the old function
    spawn_enemies();
    // создаём игрока и врагов на уровне

    derive_graphics_metrics_from_loaded_level();
    // вычисляем графические параметры уровня

    timer = MAX_LEVEL_TIME;
    // сбрасываем таймер уровня
}

// освобождает память, выделенную под уровень
void Level::unloadLevel() {
    if (levelData != nullptr) {
        delete[] levelData;
        levelData = nullptr;
    }
}

// получает ссылку на клетку уровня по координатам
char& Level::getLevelCell(size_t row, size_t column) const {
    return levelData[row * columns + column];
}

// устанавливает значение клетки уровня
void Level::setLevelCell(size_t row, size_t column, char chr) {
    getLevelCell(row, column) = chr;
}

// загружает уровень из rll файла
void Level::loadFromRLL(const std::string& filename) {
    try {
        parseRLLFile(filename);
    } catch (const std::exception& e) {
        std::cerr << "Error loading RLL file: " << e.what() << std::endl;
        throw;
    }
}

// парсит rll файл и создаёт уровень
void Level::parseRLLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // первый проход: подсчёт строк и максимального количества столбцов
    std::string line;
    rows = 0;
    columns = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // подсчитываем реальное количество символов в декодированной строке
        int rowLength = 0;
        size_t pos = 0;
        while (pos < line.length()) {
            if (std::isdigit(line[pos])) {
                int count = parseNumber(line, pos);
                if (pos < line.length()) {
                    rowLength += count;
                }
            } else {
                rowLength++;
                pos++;
            }
        }
        
        columns = std::max(columns, rowLength);
        rows++;
    }

    // выделяем память под уровень
    if (levelData != nullptr) {
        delete[] levelData;
    }
    levelData = new char[rows * columns];
    
    // инициализируем все клетки воздухом
    for (int i = 0; i < rows * columns; i++) {
        levelData[i] = AIR;
    }

    // второй проход: декодируем rll формат
    file.clear();
    file.seekg(0);
    int currentRow = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        decodeRLLRow(line, currentRow++);
    }
}

// декодирует одну строку rll формата
void Level::decodeRLLRow(const std::string& row, int currentRow) {
    size_t pos = 0;
    int currentCol = 0;
    
    while (pos < row.length()) {
        if (std::isdigit(row[pos])) {
            // парсим число повторений
            int count = parseNumber(row, pos);
            
            // получаем символ для повторения
            if (pos < row.length()) {
                char cell = row[pos++];
                // заполняем клетки
                for (int i = 0; i < count && currentCol < columns; i++) {
                    setLevelCell(currentRow, currentCol++, cell);
                }
            }
        } else {
            // одиночный символ
            setLevelCell(currentRow, currentCol++, row[pos++]);
        }
    }
}

// парсит число из строки
int Level::parseNumber(const std::string& str, size_t& pos) {
    int result = 0;
    while (pos < str.length() && std::isdigit(str[pos])) {
        result = result * 10 + (str[pos] - '0');
        pos++;
    }
    return result;
}

// тестирует функциональность уровня
bool Level::testLevelFunctionality() {
    std::cout << "Testing Level Functionality..." << std::endl;
    
    // создаём простой тестовый уровень
    const char* testLevel = "3-|@-#|3-";  // простой уровень 3x3 с игроком и стеной
    std::ofstream testFile("test_level.rll");
    testFile << testLevel;
    testFile.close();

    try {
        // загружаем тестовый уровень
        loadFromRLL("test_level.rll");
        std::cout << "✓ Level loaded successfully" << std::endl;
        
        // проверяем размеры уровня
        if (rows != 3 || columns != 3) {
            std::cout << "✗ Level dimensions incorrect" << std::endl;
            return false;
        }
        std::cout << "✓ Level dimensions correct" << std::endl;
        
        // проверяем содержимое уровня
        if (getLevelCell(0, 0) != AIR || 
            getLevelCell(1, 0) != PLAYER || 
            getLevelCell(1, 2) != WALL) {
            std::cout << "✗ Level content incorrect" << std::endl;
            return false;
        }
        std::cout << "✓ Level content correct" << std::endl;
        
        // тестируем определение столкновений
        Vector2 playerPos = {1.0f, 1.0f};  // позиция игрока
        if (!isColliding(playerPos, WALL)) {
            std::cout << "✗ Collision detection failed" << std::endl;
            return false;
        }
        std::cout << "✓ Collision detection working" << std::endl;
        
        // тестируем управление состоянием уровня
        resetLevelIndex();
        if (currentLevelIndex != 0) {
            std::cout << "✗ Level index reset failed" << std::endl;
            return false;
        }
        std::cout << "✓ Level state management working" << std::endl;
        
        // очистка
        unloadLevel();
        std::remove("test_level.rll");
        
        std::cout << "All tests passed successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with error: " << e.what() << std::endl;
        return false;
    }
}