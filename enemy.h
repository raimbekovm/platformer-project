#ifndef ENEMY_H
#define ENEMY_H

#include "globals.h"

void spawn_enemies();
void update_enemies();
bool is_colliding_with_enemies(Vector2 pos);
void remove_colliding_enemy(Vector2 pos);

#endif //ENEMY_H