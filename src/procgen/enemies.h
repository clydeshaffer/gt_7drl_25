#ifndef ENEMIES_H
#define ENEMIES_H

#include "tilemap.h"
#define MAX_ENEMIES 24
#define ENEMY_TYPE_COUNT 6

extern char enemy_types[MAX_ENEMIES];
extern char enemy_icons[MAX_ENEMIES];
extern char enemy_hp[MAX_ENEMIES];
extern char enemy_x[MAX_ENEMIES];
extern char enemy_y[MAX_ENEMIES];
extern char enemy_data[MAX_ENEMIES];
extern const char enemy_type_name[ENEMY_TYPE_COUNT];

char roll_damage(char mod);

void reset_enemies();

char add_enemy(char type, char x, char y);

void act_enemies();

#endif