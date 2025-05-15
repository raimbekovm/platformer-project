#include "level.h"

bool is_inside_level(int row, int column) {
    if (row < 0 || row >= current_level.rows) return false;
    if (column < 0 || column >= current_level.columns) return false;
    return true;
    // проверяем, находится ли клетка внутри границ уровня
}

bool is_colliding(Vector2 pos, char look_for) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс сущности для проверки столкновений

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            if (!is_inside_level(row, column)) continue;
            // пропускаем если координаты вне уровня

            if (get_level_cell(row, column) == look_for) {
                Rectangle block_hitbox = {(float) column, (float) row, 1.0f, 1.0f};
                // создаём хитбокс блока и проверяем пересечение

                if (CheckCollisionRecs(entity_hitbox, block_hitbox)) {
                    return true;
                    // если произошло столкновение — true
                }
            }
        }
    }
    return false;
    // если не было найдено столкновений — false
}

char& get_collider(Vector2 pos, char look_for) {
    Rectangle player_hitbox = {pos.x, pos.y, 1.0f, 1.0f};
    // создаём хитбокс игрока

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            if (!is_inside_level(row, column)) continue;
            // пропускаем клетки за пределами уровня

            if (get_level_cell(row, column) == look_for) {
                Rectangle block_hitbox = {(float) column, (float) row, 1.0f, 1.0f};
                // создаём хитбокс блока

                if (CheckCollisionRecs(player_hitbox, block_hitbox)) {
                    return get_level_cell(row, column);
                    // возвращаем ссылку на столкнувшуюся клетку
                }
            }
        }
    }

    return get_level_cell(pos.x, pos.y);
    // если ничего не найдено — возвращаем ссылку на текущую клетку
}

void reset_level_index() {
    level_index = 0;
    // сбрасываем индекс уровня в ноль
}

void load_level(int offset) {
    level_index += offset;
    // обновляем индекс уровня с учётом смещения

    if (level_index >= LEVEL_COUNT) {
        game_state = VICTORY_STATE;
        create_victory_menu_background();
        level_index = 0;
        return;
        // если уровни закончились — переходим к экрану победы
    }

    // копируем данные уровня
    size_t rows = LEVELS[level_index].rows;
    size_t columns = LEVELS[level_index].columns;
    current_level_data = new char[rows * columns];

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            current_level_data[row * columns + column] = LEVELS[level_index].data[row * columns + column];
        }
    }

    current_level = {rows, columns, current_level_data};
    // устанавливаем текущий уровень

    spawn_player();
    spawn_enemies();
    // создаём игрока и врагов на уровне

    derive_graphics_metrics_from_loaded_level();
    // вычисляем графические параметры уровня

    timer = MAX_LEVEL_TIME;
    // сбрасываем таймер уровня
}

void unload_level() {
    delete[] current_level_data;
    // освобождаем память, выделенную под данные уровня
}

// getters и setters
char& get_level_cell(size_t row, size_t column) {
    return current_level.data[row * current_level.columns + column];
    // получаем ссылку на клетку уровня по координатам
}

void set_level_cell(size_t row, size_t column, char chr) {
    get_level_cell(row, column) = chr;
    // устанавливаем значение клетки уровня
}