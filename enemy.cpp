#include "enemy.h"
#include "level.h"

extern Level currentLevel;

void spawn_enemies() {
    enemies.clear();
    // очищаем вектор врагов перед их новым спавном

    for (size_t row = 0; row < currentLevel.getRows(); ++row) {
        for (size_t column = 0; column < currentLevel.getColumns(); ++column) {
            char cell = currentLevel.getLevelCell(row, column);
            // получаем символ из уровня, чтобы проверить, находится ли там враг

            if (cell == ENEMY) {
                enemies.push_back({
                                          {static_cast<float>(column), static_cast<float>(row)},
                                          true
                                  });
                // добавляем врага в список, указывая его позицию и направление взгляда

                currentLevel.setLevelCell(row, column, AIR);
                // заменяем клетку на воздух, чтобы враг больше не считался частью уровня
            }
        }
    }
}

void update_enemies() {
    for (auto &enemy : enemies) {
        float next_x = enemy.pos.x;
        next_x += (enemy.is_looking_right ? ENEMY_MOVEMENT_SPEED : -ENEMY_MOVEMENT_SPEED);
        // вычисляем следующую позицию врага по x в зависимости от направления движения

        if (currentLevel.isColliding({next_x, enemy.pos.y}, WALL)) {
            enemy.is_looking_right = !enemy.is_looking_right;
            // если столкновение со стеной — меняем направление движения
        }
        else {
            enemy.pos.x = next_x;
            // иначе — обновляем позицию врага
        }
    }
}

bool is_colliding_with_enemies(Vector2 pos) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс сущности, с которой проверяем столкновение

    for (auto &enemy : enemies) {
        Rectangle enemy_hitbox = {(float) enemy.pos.x, (float) enemy.pos.y, 1.0f, 1.0f};
        // создаём хитбокс каждого врага

        if (CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            return true;
            // если хитбоксы пересекаются — есть столкновение
        }
    }
    return false;
    // если не найдено ни одного пересечения — возвращаем false
}

void remove_colliding_enemy(Vector2 pos) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс сущности для проверки столкновений с врагами

    for (auto it = enemies.begin(); it != enemies.end(); it++) {
        Rectangle enemy_hitbox = {(float) it->pos.x, (float) it->pos.y, 1.0f, 1.0f};
        // создаём хитбокс текущего врага

        if (CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            enemies.erase(it);
            // удаляем врага, если произошло столкновение

            remove_colliding_enemy(pos);
            // вызываем функцию рекурсивно на тот же хитбокс, чтобы удалить всех врагов, которые пересекаются с позицией

            return;
        }
    }
}
