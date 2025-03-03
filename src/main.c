#include "gt/gametank.h"
#include "gt/input.h"
#include "gt/gfx/draw_queue.h"
#include "procgen/tilemap.h"

char box_x = 0, box_y = 0;
char dx = 1, dy = 1;

int main () {
 
    generate_dungeon();

    setup_dungeon_render();

    while (1) {                                     //  Run forever
        update_inputs();

        if(player1_buttons & INPUT_MASK_LEFT) --box_x;
        if(player1_buttons & INPUT_MASK_RIGHT) ++box_x;
        if(player1_buttons & INPUT_MASK_UP) --box_y;
        if(player1_buttons & INPUT_MASK_DOWN) ++box_y;
        if(box_x == 255) box_x = 0;
        if(box_x == (MAP_WIDTH-MAP_DRAW_TILES+1)) --box_x;
        if(box_y == 255) box_y = 0;
        if(box_y == (MAP_WIDTH-MAP_DRAW_TILES+1)) --box_y;

        if(player1_new_buttons & INPUT_MASK_START) generate_dungeon();

        draw_dungeon(box_x, box_y);
        await_drawing();
        await_vsync(1);
        flip_pages();
 
    }
 
  return (0);                                     //  We should never get here!
}