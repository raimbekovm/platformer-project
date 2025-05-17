#ifndef LEVEL_H
#define LEVEL_H

#include "globals.h"
#include <string>
#include <fstream>
#include <stdexcept>

class Level {
private:
    int rows;
    int columns;
    char* levelData;
    int currentLevelIndex;

    // RLL parsing methods
    void parseRLLFile(const std::string& filename);
    void decodeRLLRow(const std::string& row, int currentRow);
    int parseNumber(const std::string& str, size_t& pos);

public:
    Level();
    ~Level();

    // Core functionality
    bool isInsideLevel(int row, int column) const;
    bool isColliding(Vector2 pos, char lookFor) const;
    char& getCollider(Vector2 pos, char lookFor);
    
    // Level management
    void resetLevelIndex();
    void loadLevel(int offset);
    void unloadLevel();
    
    // RLL file loading
    void loadFromRLL(const std::string& filename);
    
    // Cell access
    char& getLevelCell(size_t row, size_t column) const;
    void setLevelCell(size_t row, size_t column, char chr);
    
    // Getters
    int getRows() const { return rows; }
    int getColumns() const { return columns; }
    int getCurrentLevelIndex() const { return currentLevelIndex; }

    // Testing functionality
    bool testLevelFunctionality();
};

#endif //LEVEL_H
