#ifndef BOSSROOM_H
#define BOSSROOM_H

#define BOSS_MAP_OFFSET_X 8
#define BOSS_MAP_OFFSET_Y 8

#define BOSS_MAP_WIDTH 11
#define BOSS_MAP_HEIGHT 11
#define BOSS_TILE_COUNT (BOSS_MAP_WIDTH*BOSS_MAP_HEIGHT)

extern const char boss_tiles[BOSS_TILE_COUNT];

extern const char boss_objects[BOSS_TILE_COUNT];

#endif