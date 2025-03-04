#include "gt/gfx/draw_direct.h"
#include "procgen/tilemap.h"



extern char box_x, box_y;

char projectile_sprite;

signed char scabs(signed char x){
    if(x < 0) return -x;
    return x;
}

void draw_projectile(char x, char y) {
    if(x & 128) return;
    if(x < box_x) return;
    if(x >= (box_x + MAP_DRAW_TILES)) return;
    if(y & 128) return;
    if(y < box_y) return;
    if(y >= (box_y + MAP_DRAW_TILES)) return;
    direct_transparent_mode(1);
    DIRECT_SET_SOURCE_X((15&projectile_sprite)<<3);
    DIRECT_SET_SOURCE_Y((projectile_sprite&0xF0) >> 1);
    DIRECT_SET_WIDTH(8);
    DIRECT_SET_HEIGHT(8);
    DIRECT_SET_DEST_X(((x - box_x) << 3) + MAP_DRAW_OFFSET_X);
    DIRECT_SET_DEST_Y(((y - box_y) << 3) + MAP_DRAW_OFFSET_Y);
    DIRECT_DRAW_START();
}

//adapted from https://gist.github.com/bert/1085538
void scan_line (signed char x0, signed char y0, signed char x1, signed char y1) {
    signed char dx =  scabs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    signed char dy = -scabs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
    signed char err = dx + dy, e2; /* error value e_xy */

    if(dx > -dy) {
        if(sx == -1) projectile_sprite += 1;
    } else {
        if(sy == 1) projectile_sprite += 3;
        else projectile_sprite += 2;
    }

    for (;;){  /* loop */

        draw_dungeon(box_x, box_y);
        draw_projectile(x0, y0);
        await_vsync(1);
        flip_pages();

        if (x0 == x1 && y0 == y1) break;
        e2 = err << 1;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}