#ifndef ENEMY_H
#define ENEMY_H

#include "globals.h"
#include "level.h"
#include <stdexcept>

class Enemy {
private:
    Vector2 position;
    bool lookingRight;
    static std::vector<Enemy> enemies;

public:
    Enemy();
    Enemy(const Vector2& pos, bool lookingRight);
    
    static void spawnEnemies(const Level& level);
    static void updateEnemies();
    static bool isCollidingWithEnemies(const Vector2& pos);
    static void removeCollidingEnemy(const Vector2& pos);
    
    // Movement and state
    void update();
    void moveHorizontally(float delta);
    
    // геттер
    Vector2 getPosition() const { return position; }
    bool isLookingRight() const { return lookingRight; }
    
    // сеттер
    void setPosition(const Vector2& pos) { 
        if (pos.x < 0 || pos.y < 0) {
            throw std::invalid_argument("Invalid enemy position");
        }
        position = pos; 
    }
    void setLookingRight(bool lookingRight) { this->lookingRight = lookingRight; }

    static const std::vector<Enemy>& getEnemies() { return enemies; }
};

#endif //ENEMY_H