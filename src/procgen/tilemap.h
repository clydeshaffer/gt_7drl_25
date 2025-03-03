#ifndef TILEMAP_H
#define TILEMAP_H

#define MAP_WIDTH 32
#define TILE_WIDTH 8
#define MAP_DRAW_TILES 12
#define MAP_DRAW_WIDTH 96
#define MAPINDEX(row,col) (col + (row<<5))

extern char tilemap[MAP_WIDTH * MAP_WIDTH];
extern char object_layer[MAP_WIDTH * MAP_WIDTH];
extern char enemy_layer[MAP_WIDTH * MAP_WIDTH];

extern char player_x;
extern char player_y;

void setup_dungeon_render();

void generate_dungeon();

void draw_dungeon(char x, char y);

#endif