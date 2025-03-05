#include "../gt/feature/random/random.h"
#include "../gt/gfx/draw_direct.h"
#include "../gt/banking.h"
#include "../gen/assets/asset_main.h"
#include "../gen/bank_nums.h"
#include "tilemap.h"
#include "enemies.h"

#define BREAKS_COUNT 3
#define MIN_SIZE 4
#define ROOMS_TOTAL_WIDTH (MAP_WIDTH - (BREAKS_COUNT+2))

SpriteSlot dungeon_gfx;

char tilemap[MAP_WIDTH * MAP_WIDTH];
char object_layer[MAP_WIDTH * MAP_WIDTH];
char enemy_layer[MAP_WIDTH * MAP_WIDTH];

char player_x;
char player_y;

char h_breaks[BREAKS_COUNT+2];
char v_breaks[BREAKS_COUNT+2];
char tmp_breaks[BREAKS_COUNT+1];

char room_idx;
#define ROOM_COUNT ((BREAKS_COUNT+1) * (BREAKS_COUNT+1))
char rooms_x1[ROOM_COUNT];
char rooms_y1[ROOM_COUNT];
char rooms_x2[ROOM_COUNT];
char rooms_y2[ROOM_COUNT];
char room_partition_membership[ROOM_COUNT];
char room_partition_grouping[ROOM_COUNT];
char room_group_0_size;
char room_group_1_size;
char border_rooms[ROOM_COUNT*2];
char border_rooms_count;

char background_color_index = 0;
const char background_color_table[] = { 64, 72, 80, 88, 89, 90 };

const char loot_generation_table[] = {
    0x00, //nothing! you get nothing! you lose!
    0x61, 0x61, 0x61, 0x61,//some coins
    0x60, 0x60,//pile of coins
    0x62, 0x62, 0x62, //red potion
    0x63, //green potion
    0x64, 0x64, //blue potion
    0x66, //sword
    0x67, //bow
    0x68 //staff
};

static char i; //non reentrant shared index var

void flash_background() {
    background_color_index = sizeof(background_color_table) - 1;
}

void setup_dungeon_render() {
    dungeon_gfx = allocate_sprite(&ASSET__asset_main__tiles_bmp_load_list);
}

#pragma code-name(push, "PROG0")

char get_tile_check_bounds_slow(char x, char y) {
    if(x >= MAP_WIDTH) return 128;
    if(y >= MAP_WIDTH) return 128;
    return tilemap[MAPINDEX(y, x)] & 128;
}

char check_neighbors(char x, char y) {
    static char count, tile_cursor;
    count = 0;
    count |= !! get_tile_check_bounds_slow(x, y+1);
    count = count << 1;
    count |= !! get_tile_check_bounds_slow(x-1, y);
    count = count << 1;
    count |= !! get_tile_check_bounds_slow(x, y-1);
    count = count << 1;
    count |= !! get_tile_check_bounds_slow(x+1, y);
    return count;
}

void randomize_tmp_breaks() {
    for(i=0; i < BREAKS_COUNT+1; ++i) {
        tmp_breaks[i] = MIN_SIZE;
    }
    for(i = ROOMS_TOTAL_WIDTH - (MIN_SIZE * (BREAKS_COUNT+1)); i > 0; --i) {
        ++tmp_breaks[rnd_range(0, BREAKS_COUNT+1)];
    }

    ++tmp_breaks[0];
    for(i=1; i < BREAKS_COUNT; ++i) {
        tmp_breaks[i] += tmp_breaks[i-1] + 1;
    }
}

char do_door = 0;
void tunnel_between_points(char x1, char y1, char x2, char y2) {
    char x, y, dx, dy, axis;
    
    int ind;
    x = x1;
    y = y1;

    dx = 1;
    if(x2 < x1) dx = 255;
    dy = 1;
    if(y2 < y1) dy = 255;

    while((x!=x2) || (y!=y2)) {
        ind = MAPINDEX(y,x);
        if(do_door && (tilemap[ind] & 128)) {
            object_layer[ind] = 0x14;
            do_door = 0;
        }

        tilemap[ind] = 0;
        

        axis = rnd() & 128;
        if(x == x2) axis = 128;
        if(y == y2) axis = 0;
        if(axis) {
            y += dy;
        } else {
            x += dx;
        }
    }
}

void carve_room(char x1, char y1, char x2, char y2) {
    static char r, c, t;
    ++x1; ++y1;
    t = x2-x1;
    if(t > MIN_SIZE) {
        x2 = rnd_range(MIN_SIZE, t);
        x1 += rnd_range(0, t - x2);
        x2 += x1;
    }
    t = y2-y1;
    if(t > MIN_SIZE) {
        y2 = rnd_range(MIN_SIZE, t);
        y1 += rnd_range(0, t - y2);
        y2 += y1;
    }

    rooms_x1[room_idx] = x1;
    rooms_x2[room_idx] = x2;
    rooms_y1[room_idx] = y1;
    rooms_y2[room_idx] = y2;

    for(r = y1; r < y2; ++r) {
        for(c = x1; c < x2; ++c) {
            tilemap[MAPINDEX(r, c)] = 0;
        }
    }
}

void generate_dungeon_impl() {
    static char r, c, r2, c2;
    static char* tile_cursor;

    reset_enemies();

    tile_cursor = tilemap;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            *tile_cursor = 128;
            ++tile_cursor;
        }
    }

    tile_cursor = object_layer;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            *tile_cursor = 0;
            ++tile_cursor;
        }
    }

    tile_cursor = enemy_layer;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            *tile_cursor = 0;
            ++tile_cursor;
        }
    }

    h_breaks[0] = 0;
    v_breaks[0] = 0;
    h_breaks[BREAKS_COUNT+1] = MAP_WIDTH-1;
    v_breaks[BREAKS_COUNT+1] = MAP_WIDTH-1;

    randomize_tmp_breaks();
    //sort_tmp_breaks();
    for(i=0; i<BREAKS_COUNT;++i) {
        h_breaks[i+1] = tmp_breaks[i];
    }
    randomize_tmp_breaks();
    for(i=0; i<BREAKS_COUNT;++i) {
        v_breaks[i+1] = tmp_breaks[i];
    }

    room_idx = 0;
    for(r = 0; r < BREAKS_COUNT+1; ++r) {
        for(c = 0; c < BREAKS_COUNT+1; ++c) {
            carve_room(v_breaks[c], h_breaks[r], v_breaks[c+1], h_breaks[r+1]);
            room_partition_membership[room_idx] = 0;
            ++room_idx;
        }
    }


    r = rnd_range(0, BREAKS_COUNT+1);
    c = rnd_range(0, BREAKS_COUNT+1);
    r2 = r;
    c2 = c;
    if(r < BREAKS_COUNT) {
        r2 = rnd_range(r, BREAKS_COUNT+1);
    }
    if(c < BREAKS_COUNT) {
        c2 = rnd_range(c, BREAKS_COUNT+1);
    }

    room_idx = c;
    for(i = 0; i < r; ++i) {
        room_idx += BREAKS_COUNT+1; //somehow this is probably faster than multiplying even though its the same
    }

    while(r <= r2) {
        for(i = c; i <= c2; ++i) {
            room_partition_membership[room_idx] = 1;
            ++room_idx;
        }
        room_idx -= (c2 - c) + 1;
        room_idx += BREAKS_COUNT+1;
        ++r;
    }

    room_group_0_size = 0;
    room_group_1_size = 0;


    for(room_idx = 0; room_idx < ROOM_COUNT; ++room_idx) {
        if(room_partition_membership[room_idx]) {
            ++room_group_1_size;
            room_partition_grouping[ROOM_COUNT-room_group_1_size] = room_idx;
        } else {
            room_partition_grouping[room_group_0_size++] = room_idx;
        }
    }

    border_rooms_count = 0;

    room_idx = 0;
    for(r = 0; r < BREAKS_COUNT+1; ++r) {
        for(c = 0; c < BREAKS_COUNT+1; ++c) {
            if(c != BREAKS_COUNT) {
                if(room_partition_membership[room_idx] == room_partition_membership[room_idx+1]) {
                    tunnel_between_points(
                        rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]),
                        rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]),
                        rnd_range(rooms_x1[room_idx+1], rooms_x2[room_idx+1]),
                        rnd_range(rooms_y1[room_idx+1], rooms_y2[room_idx+1])
                    );
                } else {
                    border_rooms[border_rooms_count++] = room_idx;
                }
            }
            if(r != BREAKS_COUNT) {
                if(room_partition_membership[room_idx] == room_partition_membership[room_idx+1+BREAKS_COUNT]) {
                    tunnel_between_points(
                        rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]),
                        rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]),
                        rnd_range(rooms_x1[room_idx+1+BREAKS_COUNT], rooms_x2[room_idx+1+BREAKS_COUNT]),
                        rnd_range(rooms_y1[room_idx+1+BREAKS_COUNT], rooms_y2[room_idx+1+BREAKS_COUNT])
                    );
                } else {
                    border_rooms[border_rooms_count++] = room_idx | 128;
                }
            }
            ++room_idx;
        }
    }

    if(border_rooms_count) {
        room_idx = border_rooms[rnd_range(0, border_rooms_count)];
        c = 0;
        if(room_idx & 128) c = BREAKS_COUNT;
        do_door = 1;
        room_idx &= 127;
        tunnel_between_points(
            rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]),
            rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]),
            rnd_range(rooms_x1[room_idx+1+c], rooms_x2[room_idx+1+c]),
            rnd_range(rooms_y1[room_idx+1+c], rooms_y2[room_idx+1+c])
        );
    }

    for(room_idx = 1; room_idx < ROOM_COUNT; ++room_idx) {
        if((rnd() & 3) == 0) c = loot_generation_table[rnd_range(0, sizeof(loot_generation_table))];
        else c = 0x50 + rnd_range(0, 6);

        object_layer[MAPINDEX(rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]),rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]))] = c;

        if((rnd() & 3) == 0) {
            if((rnd() & 3) == 0) c = loot_generation_table[rnd_range(0, sizeof(loot_generation_table))];
            else c = 0x50 + rnd_range(0, 6);
            object_layer[MAPINDEX(rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]),rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]))] = c;
        }
    }

    tile_cursor = object_layer;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            if((*tile_cursor & 0xF0) == 0x50) {
                enemy_layer[MAPINDEX(r, c)] = add_enemy(*tile_cursor & 0x0F, c, r);
                *tile_cursor = 0;
            }
            ++tile_cursor;
        }
    }

    if(room_group_0_size) {
        room_idx = room_partition_grouping[rnd_range(0, room_group_0_size)];
        c = rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]);
        r = rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]);
        object_layer[MAPINDEX(r, c)] = 0x65;


        r =  room_idx;
        while(room_idx == r) {
            //dont put key and player start in same room to make sure player doesn't overwrite key
            room_idx = room_partition_grouping[rnd_range(0, room_group_0_size)];
        }
    } else {
        room_idx = rnd_range(0, ROOM_COUNT);
    }
    
    player_x = rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]);
    player_y = rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]);

    object_layer[MAPINDEX(player_y, player_x)] = 0x40;

    r = enemy_layer[MAPINDEX(player_y, player_x)];
    if(r) {
        enemy_hp[r-1] = 0;
        enemy_layer[MAPINDEX(player_y, player_x)] = 0;
    }
    
    room_idx = room_partition_grouping[rnd_range(ROOM_COUNT - room_group_1_size, ROOM_COUNT)];
    c = rnd_range(rooms_x1[room_idx], rooms_x2[room_idx]);
    r = rnd_range(rooms_y1[room_idx], rooms_y2[room_idx]);
    object_layer[MAPINDEX(r, c)] = 0x10;

    tile_cursor = tilemap;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            if(*tile_cursor) {
                *tile_cursor |= check_neighbors(c, r) << 3;
            }
            ++tile_cursor;
        }
    }

    tile_cursor = tilemap;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            if(!(*tile_cursor & 128)) {
                *tile_cursor = 120;
            }
            *tile_cursor ^= 120;
            ++tile_cursor;
        }
    }

    tile_cursor = object_layer;
    for(r = 0; r < MAP_WIDTH; ++r) {
        for(c = 0; c < MAP_WIDTH; ++c) {
            if(*tile_cursor == 0x14) {
                tilemap[MAPINDEX(r,c)] = 128;
            }
            ++tile_cursor;
        }
    }
}

#pragma code-name (pop)

void generate_dungeon() {
    push_rom_bank();
    change_rom_bank(BANK_PROG0);
    generate_dungeon_impl();
    pop_rom_bank();
}

void draw_dungeon(char x, char y) {
    static char r, c, t;
    static char* tile_cursor, *tile_cursor2;
    tile_cursor = tilemap + MAPINDEX(y, x);
    direct_prepare_sprite_mode(dungeon_gfx);
    direct_transparent_mode(0);
    DIRECT_SET_SOURCE_Y(0);
    DIRECT_SET_WIDTH(TILE_WIDTH);
    DIRECT_SET_HEIGHT(TILE_WIDTH);
    DIRECT_SET_COLOR(background_color_table[background_color_index]);
    if(background_color_index) {
        --background_color_index;
    }
    for(r = 0; r < MAP_DRAW_WIDTH; r+=TILE_WIDTH) {
        DIRECT_SET_DEST_Y(r+MAP_DRAW_OFFSET_Y);
        for(c = 0; c < MAP_DRAW_WIDTH; c+=TILE_WIDTH) {
            t = *tile_cursor;
            await_drawing();
            if((t&128) == 0) {
                flagsMirror |= DMA_COLORFILL_ENABLE;
            } else {
                flagsMirror &= ~DMA_COLORFILL_ENABLE;
            }
            *dma_flags = flagsMirror;
            DIRECT_SET_DEST_X(c+MAP_DRAW_OFFSET_X);
            DIRECT_SET_SOURCE_X(t&127);
            DIRECT_DRAW_START();
            ++tile_cursor;
        }
        tile_cursor -= MAP_DRAW_TILES;
        tile_cursor += MAP_WIDTH;
    }

    tile_cursor = object_layer + MAPINDEX(y, x);
    tile_cursor2 = enemy_layer + MAPINDEX(y, x);
    flagsMirror &= ~DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    direct_transparent_mode(1);
    for(r = 0; r < MAP_DRAW_WIDTH; r+=TILE_WIDTH) {
        DIRECT_SET_DEST_Y(r+MAP_DRAW_OFFSET_Y);
        for(c = 0; c < MAP_DRAW_WIDTH; c+=TILE_WIDTH) {
            t = *tile_cursor2;
            if(t) {
                t = enemy_icons[t-1];
                await_drawing();
                DIRECT_SET_DEST_X(c+MAP_DRAW_OFFSET_X);
                DIRECT_SET_SOURCE_X((15&t)<<3);
                DIRECT_SET_SOURCE_Y((t&0xF0) >> 1);
                DIRECT_DRAW_START();
            } else {
                t = *tile_cursor;
                if(t) {
                    await_drawing();
                    DIRECT_SET_DEST_X(c+MAP_DRAW_OFFSET_X);
                    DIRECT_SET_SOURCE_X((15&t)<<3);
                    DIRECT_SET_SOURCE_Y((t&0xF0) >> 1);
                    DIRECT_DRAW_START();
                }
            }
            ++tile_cursor;
            ++tile_cursor2;
        }
        tile_cursor -= MAP_DRAW_TILES;
        tile_cursor += MAP_WIDTH;
        tile_cursor2 -= MAP_DRAW_TILES;
        tile_cursor2 += MAP_WIDTH;
    }
}