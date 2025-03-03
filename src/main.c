#include "gt/gametank.h"
#include "gt/input.h"
#include "gt/gfx/draw_queue.h"
#include "procgen/tilemap.h"

char box_x = 0, box_y = 0;
char old_x = 1, old_y = 1;

int main () {
 
    generate_dungeon();

    setup_dungeon_render();

    while (1) {                                     //  Run forever
        update_inputs();

        old_x = player_x;
        old_y = player_y;

        if(player1_new_buttons & INPUT_MASK_ANY_DIRECTION) {
            object_layer[MAPINDEX(player_y, player_x)] = 0;
        }

        if(player1_new_buttons & INPUT_MASK_LEFT) --player_x;
        else if(player1_new_buttons & INPUT_MASK_RIGHT) ++player_x;
        else if(player1_new_buttons & INPUT_MASK_UP) --player_y;
        else if(player1_new_buttons & INPUT_MASK_DOWN) ++player_y;

        if(player1_new_buttons & INPUT_MASK_ANY_DIRECTION) {

            if(tilemap[MAPINDEX(player_y, player_x)] & 128) {
                player_x = old_x;
                player_y = old_y;
            }
            else if(object_layer[MAPINDEX(player_y, player_x)]) {
                player_x = old_x;
                player_y = old_y;
            }

            object_layer[MAPINDEX(player_y, player_x)] = 0x40;
        }

        if((player_x - box_x) > 8) ++box_x;
        if((player_x - box_x) < 4) --box_x;
        if((player_y - box_y) > 8) ++box_y;
        if((player_y - box_y) < 4) --box_y;

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