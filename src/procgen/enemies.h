#ifndef ENEMIES_H
#define ENEMIES_H

#include "tilemap.h"
#define MAX_ENEMIES 24
#define ENEMY_TYPE_COUNT 6

extern char enemy_types[MAX_ENEMIES];
extern char enemy_icons[MAX_ENEMIES];

void reset_enemies();

char add_enemy(char type, char x, char y);

void act_enemies();

#endif