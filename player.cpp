#include "player.h"

void move_player_horizontally(float delta) {
    float next_x = player_pos.x + delta;
    if (!is_colliding({next_x, player_pos.y}, WALL)) {
        player_pos.x = next_x;
    } else {
        player_pos.x = roundf(player_pos.x);
        return;
    }

    is_looking_forward = delta > 0;
    if (delta != 0) is_moving = true;
}

void update_player_gravity() {
    if (is_colliding({player_pos.x, player_pos.y - 0.1f}, WALL) && player_y_velocity < 0) {
        player_y_velocity = CEILING_BOUNCE_OFF;
    }

    player_pos.y += player_y_velocity;
    player_y_velocity += GRAVITY_FORCE;

    is_player_on_ground = is_colliding({player_pos.x, player_pos.y + 0.1f}, WALL);
    if (is_player_on_ground) {
        player_y_velocity = 0;
        player_pos.y = roundf(player_pos.y);
    }
}

void update_player() {
    update_player_gravity();

    if (is_colliding(player_pos, COIN)) {
        get_collider(player_pos, COIN) = AIR;
        increment_player_score();
    }

    if (is_colliding(player_pos, EXIT)) {
        if (timer > 0) {
            timer -= 25;
            time_to_coin_counter += 5;

            if (time_to_coin_counter / 60 > 1) {
                increment_player_score();
                time_to_coin_counter = 0;
            }
        } else {
            load_level(1);
            PlaySound(exit_sound);
        }
    } else {
        if (timer >= 0) timer--;
    }

    if (is_colliding(player_pos, SPIKE) || player_pos.y > current_level.rows) {
        kill_player();
    }

    if (is_colliding_with_enemies(player_pos)) {
        if (player_y_velocity > 0) {
            remove_colliding_enemy(player_pos);
            PlaySound(kill_enemy_sound);
            increment_player_score();
            player_y_velocity = -BOUNCE_OFF_ENEMY;
        } else {
            kill_player();
        }
    }
}
