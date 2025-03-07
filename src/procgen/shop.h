#ifndef SHOP_H
#define SHOP_H

#define SHOP_ITEM_LOW_PRICE 0xFE
#define STOP_ITEM_HIGH_PRICE 0xFF

#define SHOP_MAP_OFFSET_X 9
#define SHOP_MAP_OFFSET_Y 11

#define SHOP_WIDTH 13
#define SHOP_HEIGHT 10
#define SHOP_TILE_COUNT (SHOP_WIDTH*SHOP_HEIGHT)

#define SHOP_LOW_PRICE_ITEMS_COUNT 9
#define SHOP_HIGH_PRICE_ITEMS_COUNT 12
extern const char shop_items_low_price_list[SHOP_LOW_PRICE_ITEMS_COUNT];
extern const char shop_items_high_price_list[SHOP_HIGH_PRICE_ITEMS_COUNT];

extern const char shop_tiles[SHOP_TILE_COUNT];

extern const char shop_objects[SHOP_TILE_COUNT];

#endif