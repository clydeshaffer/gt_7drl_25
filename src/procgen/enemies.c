#include "enemies.h"
#include "tilemap.h"
#include "../gt/feature/random/random.h"

#include "../logtext.h"
#include "../gen/assets/asset_main/words.json.h"

char enemy_types[MAX_ENEMIES];
char enemy_icons[MAX_ENEMIES];
char enemy_hp[MAX_ENEMIES];
char enemy_x[MAX_ENEMIES];
char enemy_y[MAX_ENEMIES];
char enemy_data[MAX_ENEMIES];

char enemy_count;

const char enemy_type_icons[ENEMY_TYPE_COUNT] = {0x50, 0x51, 0x52, 0x53, 0x54, 0x55};
const char enemy_type_initial_hp[ENEMY_TYPE_COUNT] = { 4, 5, 5, 3, 6, 8 };

#define ATTACK_TABLE_SIZE 16
const char enemy_attack_dmg_table[ATTACK_TABLE_SIZE] = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3};

const char enemy_type_name[ENEMY_TYPE_COUNT] = {
    WORDS_TAG_BEETLE_START,
    WORDS_TAG_GOBLIN_START,
    WORDS_TAG_ZOMBIE_START,
    WORDS_TAG_GHOST_START,
    WORDS_TAG_NAGA_START,
    WORDS_TAG_OGRE_START
};

char enemy_idx;

extern char player_hp;

void reset_enemies() {
    enemy_count = 0;
    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        enemy_hp[enemy_idx] = 0;
    }
}

char add_enemy(char type, char x, char y) {
    if(enemy_count == MAX_ENEMIES) {
        return 0;
    }

    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        if(enemy_hp[enemy_idx] == 0) {
            enemy_types[enemy_idx] = type;
            enemy_icons[enemy_idx] = enemy_type_icons[type];
            enemy_hp[enemy_idx] = enemy_type_initial_hp[type];
            enemy_x[enemy_idx] = x;
            enemy_y[enemy_idx] = y;
            enemy_data[enemy_idx] = 0;
            ++enemy_count;
            return enemy_idx+1;
        }
    }
    
    return 0;
}

void act_enemies() {
    static char tx, ty, dmg;
    static int tmpidx;
    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        if(enemy_hp[enemy_idx] != 0) {
            tx = enemy_x[enemy_idx];
            ty = enemy_y[enemy_idx];
            switch(rnd_range(0, 4)) {
                case 0:
                    ++tx;
                    break;
                case 1:
                    --tx;
                    break;
                case 2:
                    ++ty;
                    break;
                case 3:
                    --ty;
                    break;
            }
            tmpidx = MAPINDEX(ty, tx);
            if(!(tilemap[tmpidx] & 128) && !(enemy_layer[tmpidx])) {
                if((object_layer[tmpidx] & 0xF0) == 0x40) {
                    dmg = enemy_attack_dmg_table[rnd_range(0, ATTACK_TABLE_SIZE)];
                    player_hp -= dmg;
                    if(player_hp & 128) player_hp = 0;
                    if(player_hp == 0) {
                        dmg = WORDS_TAG_SLAIN_START;
                    }
                    push_log(WORDS_TAG_MISSED_START + dmg, WORDS_TAG_BY_START, enemy_type_name[enemy_types[enemy_idx]]);
                } else {
                    enemy_layer[MAPINDEX(enemy_y[enemy_idx], enemy_x[enemy_idx])] = 0;
                    enemy_x[enemy_idx] = tx;
                    enemy_y[enemy_idx] = ty;
                    enemy_layer[tmpidx] = enemy_idx+1;
                }
            }
        }
    }
}