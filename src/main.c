#include "gt/gametank.h"
#include "gt/input.h"
#include "gt/gfx/draw_queue.h"
#include "gt/gfx/draw_direct.h"
#include "procgen/tilemap.h"
#include "procgen/enemies.h"
#include "logtext.h"
#include "gen/assets/asset_main/words.json.h"

char box_x = 0, box_y = 0;
char old_x = 1, old_y = 1;
char hit_obj;
char player_icon;
char key_count;
char player_hp;
char player_mp;
int money_count;
int tile_idx;

void draw_ui() {
    await_drawing();
    flagsMirror |= DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    DIRECT_SET_COLOR(32);
    DIRECT_SET_DEST_X(MAP_DRAW_OFFSET_X+MAP_DRAW_WIDTH);
    DIRECT_SET_DEST_Y(MAP_DRAW_OFFSET_Y);
    DIRECT_SET_HEIGHT(MAP_DRAW_WIDTH);
    DIRECT_SET_WIDTH(2*TILE_WIDTH);
    DIRECT_DRAW_START();
    await_drawing();
    direct_tiled_mode(1);
    flagsMirror &= ~DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    DIRECT_SET_SOURCE_X(0);
    DIRECT_SET_SOURCE_Y(112);
    DIRECT_SET_WIDTH(4);
    DIRECT_SET_DEST_X(MAP_DRAW_OFFSET_Y+MAP_DRAW_WIDTH+2);
    DIRECT_SET_DEST_Y((MAP_DRAW_WIDTH + MAP_DRAW_OFFSET_X) - (player_hp << 2));
    DIRECT_SET_HEIGHT(player_hp << 2);
    DIRECT_DRAW_START();
    await_drawing();
    DIRECT_SET_DEST_X(MAP_DRAW_OFFSET_Y+MAP_DRAW_WIDTH+6);
    DIRECT_SET_SOURCE_X(4);
    DIRECT_SET_DEST_Y((MAP_DRAW_WIDTH + MAP_DRAW_OFFSET_X) - (player_mp << 2));
    DIRECT_SET_HEIGHT(player_mp << 2);
    DIRECT_DRAW_START();
    if(key_count) {
        await_drawing();
        DIRECT_SET_DEST_Y(MAP_DRAW_OFFSET_Y);
        DIRECT_SET_DEST_X(MAP_DRAW_OFFSET_Y+MAP_DRAW_WIDTH);
        DIRECT_SET_SOURCE_X(8);
        DIRECT_SET_SOURCE_Y(112);
        DIRECT_SET_WIDTH(8);
        DIRECT_SET_HEIGHT(key_count << 3);
        DIRECT_DRAW_START();
    }
}

int main () {

    generate_dungeon();
    

    setup_dungeon_render();
    prepare_log_text();

    player_icon = 0x40;
    key_count = 0;
    money_count = 0;
    player_hp = 8;
    player_mp = 2;

    while (1) {                                     //  Run forever
        update_inputs();

        old_x = player_x;
        old_y = player_y;

        if(player_hp) {
            if(player1_new_buttons & INPUT_MASK_ANY_DIRECTION) {
                object_layer[MAPINDEX(player_y, player_x)] = 0;
            }

            if(player1_new_buttons & INPUT_MASK_LEFT) --player_x;
            else if(player1_new_buttons & INPUT_MASK_RIGHT) ++player_x;
            else if(player1_new_buttons & INPUT_MASK_UP) --player_y;
            else if(player1_new_buttons & INPUT_MASK_DOWN) ++player_y;

            if(player1_new_buttons & INPUT_MASK_ANY_DIRECTION) {
                tile_idx = MAPINDEX(player_y, player_x);
                if (enemy_layer[tile_idx]) {
                    player_x = old_x;
                    player_y = old_y;
                } else if(tilemap[tile_idx] & 128) {
                    if((object_layer[tile_idx] == 0x14) && key_count) {
                        --key_count;
                        tilemap[tile_idx] = 0;
                        push_log(WORDS_TAG_UNLOCKED_START, WORDS_TAG_DOOR_START, 255);
                    } else {
                        player_x = old_x;
                        player_y = old_y;
                    }
                }
                else{
                    hit_obj = object_layer[tile_idx];
                    if(hit_obj) {
                        if((hit_obj & 0xF0) == 0x60) {
                            switch(hit_obj) {
                                case 0x66:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_SWORD_START, 255);
                                    player_icon = 0x41;
                                    break;
                                case 0x67:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_LONGBOW_START, 255);
                                    player_icon = 0x42;
                                    break;
                                case 0x68:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_STAFF_START, 255);
                                    player_icon = 0x43;
                                    break;
                                case 0x60:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_GOLD_PILE_START, 255);
                                    money_count += 15;
                                    break;
                                case 0x61:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_COINS_START, 255);
                                    money_count += 3;
                                    break;
                                case 0x65:
                                    ++key_count;
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_KEY_START, 255);
                                    break;
                                case 0x62:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_RED_POTION_START, 255);
                                    ++player_hp;
                                    break;
                                case 0x63:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_GREEN_POTION_START, 255);
                                    break;
                                case 0x64:
                                    push_log(WORDS_TAG_PICKED_UP_START, WORDS_TAG_BLUE_POTION_START, 255);
                                    ++player_mp;
                                    break;
                            }
                        } else {
                            player_x = old_x;
                            player_y = old_y;
                        }
                    }
                }

                object_layer[MAPINDEX(player_y, player_x)] = player_icon;

                act_enemies();
            }
        } else {
            player_icon = 0x44;
            object_layer[MAPINDEX(player_y, player_x)] = player_icon;
        }

        if((player_x - box_x) > 8) ++box_x;
        if((player_x - box_x) < 4) --box_x;
        if((player_y - box_y) > 8) ++box_y;
        if((player_y - box_y) < 4) --box_y;

        if(box_x == 255) box_x = 0;
        if(box_x == (MAP_WIDTH-MAP_DRAW_TILES+1)) --box_x;
        if(box_y == 255) box_y = 0;
        if(box_y == (MAP_WIDTH-MAP_DRAW_TILES+1)) --box_y;

        if(player1_new_buttons & INPUT_MASK_START) {
            generate_dungeon();
        }

        draw_dungeon(box_x, box_y);

        draw_ui();
        await_drawing();

        show_logs(MAP_DRAW_OFFSET_X, MAP_DRAW_OFFSET_Y + MAP_DRAW_WIDTH + 2, 2);
        await_draw_queue();

        await_vsync(1);
        flip_pages();
 
    }
 
  return (0);                                     //  We should never get here!
}