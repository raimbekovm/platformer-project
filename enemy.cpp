#include "enemy.h"
#include "level.h"

extern Level currentLevel;

std::vector<Enemy> Enemy::enemies;

Enemy::Enemy() : position({0, 0}), lookingRight(true) {}

Enemy::Enemy(const Vector2& pos, bool lookingRight)
    : position(pos), lookingRight(lookingRight) {}

// размещает врагов на уровне
void Enemy::spawnEnemies(const Level& level) {
    enemies.clear();
    
    for (size_t row = 0; row < level.getRows(); ++row) {
        for (size_t column = 0; column < level.getColumns(); ++column) {
            char cell = level.getLevelCell(row, column);
            
            if (cell == ENEMY) {
                enemies.push_back(Enemy(
                    {static_cast<float>(column), static_cast<float>(row)},
                    true
                ));
                
                const_cast<Level&>(level).setLevelCell(row, column, AIR);
            }
        }
    }
}

// обновляет состояние всех врагов
void Enemy::updateEnemies() {
    for (auto& enemy : enemies) {
        enemy.update();
    }
}

// проверяет столкновение с врагом
bool Enemy::isCollidingWithEnemies(const Vector2& pos) {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};
    
    for (const auto& enemy : enemies) {
        Rectangle enemyHitbox = {
            enemy.position.x,
            enemy.position.y,
            1.0f,
            1.0f
        };
        
        if (CheckCollisionRecs(entityHitbox, enemyHitbox)) {
            return true;
        }
    }
    return false;
}

// удаляет врага с которым произошло столкновение
void Enemy::removeCollidingEnemy(const Vector2& pos) {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};
    
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        Rectangle enemyHitbox = {
            it->position.x,
            it->position.y,
            1.0f,
            1.0f
        };
        
        if (CheckCollisionRecs(entityHitbox, enemyHitbox)) {
            enemies.erase(it);
            return;
        }
    }
}

// обновляет состояние отдельного врага
void Enemy::update() {
    moveHorizontally(lookingRight ? ENEMY_MOVEMENT_SPEED : -ENEMY_MOVEMENT_SPEED);
}

// обрабатывает горизонтальное движение врага с проверкой столкновений
void Enemy::moveHorizontally(float delta) {
    float next_x = position.x + delta;
    
    if (currentLevel.isColliding({next_x, position.y}, WALL)) {
        lookingRight = !lookingRight;
    } else {
        position.x = next_x;
    }
}