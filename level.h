#ifndef LEVEL_H
#define LEVEL_H

#include "globals.h"

// Function declarations
bool is_inside_level(int row, int column);
bool is_colliding(Vector2 pos, char look_for);
char& get_collider(Vector2 pos, char look_for);
void reset_level_index();
void load_level(int offset);
void unload_level();
char& get_level_cell(size_t row, size_t column);
void set_level_cell(size_t row, size_t column, char chr);

#endif //LEVEL_H
