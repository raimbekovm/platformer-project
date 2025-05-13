#include "player.h"

void reset_player_stats() {
    player_lives = MAX_PLAYER_LIVES;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        player_level_scores[i] = 0;
    }
}

void increment_player_score() {
    PlaySound(coin_sound);
    player_level_scores[level_index]++;
}

int get_total_player_score() {
    int sum = 0;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        sum += player_level_scores[i];
    }
    return sum;
}

void spawn_player() {
    player_y_velocity = 0;
    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t column = 0; column < current_level.columns; ++column) {
            char cell = get_level_cell(row, column);
            if (cell == PLAYER) {
                player_pos.x = column;
                player_pos.y = row;
                set_level_cell(row, column, AIR);
                return;
            }
        }
    }
}

void kill_player() {
    PlaySound(player_death_sound);
    game_state = DEATH_STATE;
    player_lives--;
    player_level_scores[level_index] = 0;
}
