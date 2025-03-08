#include "shop.h"
#define O 0x00 //empty
#define X 0x80 //wall
#define S 0x40 //Start
#define E 0x10 //Exit
#define K 0x18 //shopKeeper
#define C 0x19 //cat
#define BC 0x20 //Big Cat
#define T 0x16 // Ten coin sign
#define N 0x17 //Ninetynine coin sign
#define L SHOP_ITEM_LOW_PRICE
#define H STOP_ITEM_HIGH_PRICE

#pragma rodata-name ("PROG0")

const char shop_items_low_price_list[SHOP_LOW_PRICE_ITEMS_COUNT] = { 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, };
const char shop_items_high_price_list[SHOP_HIGH_PRICE_ITEMS_COUNT] = { 0x65, 0x66, 0x67, 0x68, 0x65, 0x66, 0x67, 0x68, 0x65, 0x66, 0x67, 0x68, };

const char shop_tiles[SHOP_TILE_COUNT] = {
    X, X, X, X, O, X, O, X, O, X, X, X, X,
    X, X, X, X, O, X, O, X, O, X, X, X, X,
    X, X, X, O, O, O, O, O, O, O, X, X, X,
    X, X, X, O, O, O, O, O, O, O, X, X, X,
    X, X, X, O, O, O, O, O, O, O, X, X, X,
    X, X, X, O, O, O, O, O, O, O, X, X, X,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
};

const char shop_objects[SHOP_TILE_COUNT] = {
    O, O, O, O, L, O, L, O, H, O, O, O, O,
    O, O, O, O, T, O, T, O, N, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O,BC, K, C, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
    O, S, O, O, O, O, O, O, O, O, O, E, O,
    O, O, O, O, O, O, O, O, O, O, O, O, O,
};