#include "player.h"

void reset_player_stats() {
    player_lives = MAX_PLAYER_LIVES;
    // сбрасываем количество жизней игрока

    for (int i = 0; i < LEVEL_COUNT; i++) {
        player_level_scores[i] = 0;
        // обнуляем очки на каждом уровне
    }
}

void increment_player_score() {
    PlaySound(coin_sound);
    // звук при сборе монеты

    player_level_scores[level_index]++;
    // увеличиваем счёт на текущем уровне
}

int get_total_player_score() {
    int sum = 0;

    for (int i = 0; i < LEVEL_COUNT; i++) {
        sum += player_level_scores[i];
        // суммируем очки со всех уровней
    }

    return sum;
}

void spawn_player() {
    player_y_velocity = 0;
    // обнуляем вертикальную скорость игрока

    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t column = 0; column < current_level.columns; ++column) {
            char cell = get_level_cell(row, column);

            if (cell == PLAYER) {
                player_pos.x = column;
                player_pos.y = row;
                set_level_cell(row, column, AIR);
                // устанавливаем позицию игрока и очищаем клетку
                return;
            }
        }
    }
}

void kill_player() {
    PlaySound(player_death_sound);
    // звук смерти игрока

    game_state = DEATH_STATE;
    // состояние игры смерть

    player_lives--;
    // уменьшаем количество жизней

    player_level_scores[level_index] = 0;
    // обнуляем очки на текущем уровне
}

void move_player_horizontally(float delta) {
    float next_x = player_pos.x + delta;
    // рассчитываем следующую позицию по X

    if (!is_colliding({next_x, player_pos.y}, WALL)) {
        player_pos.x = next_x;
        // если нет столкновений — перемещаем игрока
    }
    else {
        player_pos.x = roundf(player_pos.x);
        // если столкновение — выравниваем позицию
        return;
    }

    is_looking_forward = delta > 0;
    // обновляем направление взгляда

    if (delta != 0) is_moving = true;
    // если игрок двигается — ставим флаг движения
}

void update_player_gravity() {
    if (is_colliding({player_pos.x, player_pos.y - 0.1f}, WALL) && player_y_velocity < 0) {
        player_y_velocity = CEILING_BOUNCE_OFF;
        // отскакиваем от потолка
    }

    player_pos.y += player_y_velocity;
    // перемещаем игрока по оси Y

    player_y_velocity += GRAVITY_FORCE;
    // применяем силу гравитации

    is_player_on_ground = is_colliding({player_pos.x, player_pos.y + 0.1f}, WALL);
    // проверяем, стоит ли игрок на земле

    if (is_player_on_ground) {
        player_y_velocity = 0;
        player_pos.y = roundf(player_pos.y);
        // если стоит — сбрасываем скорость и выравниваем позицию
    }
}

void update_player() {
    update_player_gravity();
    // обновляем положение игрока по гравитации

    if (is_colliding(player_pos, COIN)) {
        get_collider(player_pos, COIN) = AIR;
        // удаляем монету с уровня

        increment_player_score();
        // добавляем очки игроку
    }

    if (is_colliding(player_pos, EXIT)) {
        if (timer > 0) {
            timer -= 25;
            time_to_coin_counter += 5;
            // уменьшаем таймер и увеличиваем счётчик монеты

            if (time_to_coin_counter / 60 > 1) {
                increment_player_score();
                time_to_coin_counter = 0;
                // через определённое время начисляем очки
            }
        }
        else {
            load_level(1);
            PlaySound(exit_sound);
            // если время вышло — загружаем следующий уровень
        }
    }
    else {
        if (timer >= 0) timer--;
        // уменьшаем таймер при отсутствии выхода
    }

    if (is_colliding(player_pos, SPIKE) || player_pos.y > current_level.rows) {
        kill_player();
        // если накололся на шипы или упал вниз — игрок умирает
    }

    if (is_colliding_with_enemies(player_pos)) {
        if (player_y_velocity > 0) {
            remove_colliding_enemy(player_pos);
            PlaySound(kill_enemy_sound);

            increment_player_score();
            player_y_velocity = -BOUNCE_OFF_ENEMY;
            // если прыгнул на врага — убиваем врага и подпрыгиваем
        }
        else {
            kill_player();
            // иначе — игрок умирает от врага
        }
    }
}