#ifndef ENEMIES_H
#define ENEMIES_H

#include "tilemap.h"
#define MAX_ENEMIES 24
#define ENEMY_TYPE_COUNT 18

#define ENEMY_MOVEMENT_SIT 0 //dont move, "chase" only if distance=1
#define ENEMY_MOVEMENT_WANDER 1 //pick a direction randomly
#define ENEMY_MOVEMENT_CHASE 2 //move towards player along most distant axis
#define ENEMY_MOVEMENT_RETREAT 3 //with distance limit
#define ENEMY_BITFIELD_MOVEMENT 0b00000011 //mask for enemy movement bitfield

#define ENEMY_AGGRO_PASSIVE 0 //wont auto to chase mode
#define ENEMY_AGGRO_PROXIMITY 4 //switch to chase mode in certain distance
#define ENEMY_AGGRO_DEFENSIVELY 8 //if damaged or nearby enemy is damaged
#define ENEMY_AGGRO_SOON 12 //some % chance of switching to chase on a given turn
#define ENEMY_BITFIELD_AGGRO 0b00001100 //mask for enemy aggro bitfield

#define ENEMY_FLAG_CUTE 16 //cannot harm or be harmed, "attacks" show petting text
#define ENEMY_FLAG_HIT_AND_RUN 32 //change to retreat mode after attacking
#define ENEMY_FLAG_RANGED_ATTACKS 64 //use ranged attacks periodically
#define ENEMY_FLAG_WANDERWALL 128 //switch to wander when hitting a wall
#define ENEMY_BITFIELD_FLAGS 0b11110000

#define ENEMY_PROXIMITY_AGGRO_RANGE 6

extern char enemy_types[MAX_ENEMIES];
extern char enemy_icons[MAX_ENEMIES];
extern char enemy_hp[MAX_ENEMIES];
extern char enemy_x[MAX_ENEMIES];
extern char enemy_y[MAX_ENEMIES];
extern char enemy_data[MAX_ENEMIES];
extern const char enemy_type_name[ENEMY_TYPE_COUNT];
extern const char enemy_type_defense_modifiers[ENEMY_TYPE_COUNT];
extern char enemy_closest_idx;
extern char enemy_closest_dist;

char roll_damage(signed char mod);

void damage_enemy(char enemy_id, char dmg);

void reset_enemies();

char add_enemy(char type, char x, char y);

void act_enemies();

#endif