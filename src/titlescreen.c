#include "gt/gfx/draw_queue.h"
#include "gen/assets/asset_main.h"

SpriteSlot title_graphics;

void init_titlescreen() {
    title_graphics = allocate_sprite(&ASSET__asset_main__titlecard_bmp_load_list);
}

void draw_titlescreen() {    
    queue_draw_sprite(0, 0, 127, 127, 0, 0, title_graphics);
}