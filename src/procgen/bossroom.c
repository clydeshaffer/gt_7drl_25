#include "bossroom.h"
#define O 0x00
#define X 0x80
#define S 0x40
#define R 0x5D
#define F 0x5E
#define L 0x5F

#pragma rodata-name ("PROG0")

const char boss_tiles[BOSS_TILE_COUNT] = {
    O, O, O, O, O, O, O, O, O, O, O,
    O, X, X, O, O, O, O, O, X, X, O,
    O, X, O, O, O, O, O, O, O, X, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, X, O, O, O, O, O, O, O, X, O,
    O, X, X, O, O, O, O, O, X, X, O,
    O, O, O, O, O, O, O, O, O, O, O,
};

const char boss_objects[BOSS_TILE_COUNT] = {
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, R, F, L, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, S, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O,
};