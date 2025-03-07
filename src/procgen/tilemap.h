#ifndef TILEMAP_H
#define TILEMAP_H

#define MAP_WIDTH 32
#define MAP_DRAW_OFFSET_X 8
#define MAP_DRAW_OFFSET_Y 8
#define TILE_WIDTH 8
#define MAP_DRAW_TILES 12
#define MAP_DRAW_WIDTH 96
#define MAPINDEX(row,col) (col + (row<<5))

#define IS_SHOP_FLOOR (floor_num == 5)
#define IS_ONE_AFTER_SHOP_FLOOR (floor_num == 6)

extern char tilemap[MAP_WIDTH * MAP_WIDTH];
extern char object_layer[MAP_WIDTH * MAP_WIDTH];
extern char enemy_layer[MAP_WIDTH * MAP_WIDTH];

extern char player_x, player_y;
extern char player_old_x, player_old_y;

void flash_background();

void setup_dungeon_render();

void generate_dungeon();

void draw_dungeon(char x, char y);

extern char floor_num;
extern char floor_tens;
extern char floor_ones;

void reset_floor_number();
void inc_floor_number();

#endif