#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"

// Function declarations
void reset_player_stats();
void increment_player_score();
int get_total_player_score();
void spawn_player();
void kill_player();
void move_player_horizontally(float delta);
void update_player_gravity();
void update_player();

#endif // PLAYER_H
