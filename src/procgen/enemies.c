#include "enemies.h"
#include "tilemap.h"
#include "../gt/feature/random/random.h"
#include "../gt/audio/music.h"

#include "../logtext.h"
#include "../buffs.h"
#include "../gen/assets/asset_main/words.json.h"
#include "../gen/assets/asset_main.h"
#include "../gen/bank_nums.h"
#include "../gt/banking.h"

char enemy_types[MAX_ENEMIES];
char enemy_icons[MAX_ENEMIES];
char enemy_hp[MAX_ENEMIES];
char enemy_x[MAX_ENEMIES];
char enemy_y[MAX_ENEMIES];
char enemy_data[MAX_ENEMIES];
char enemy_turn_counter[MAX_ENEMIES];
char enemy_count;

const char enemy_type_initial_hp[ENEMY_TYPE_COUNT] =        { 2, 8, 5, 8, 4, 10, 10, 20, 8, 40, 50, 50, 60, 66, 80, 66, 99, 99 };
const char enemy_type_attack_modifiers[ENEMY_TYPE_COUNT] =  { 0, 0, 0, 1, 1,  1,  2,  2, 2,  3,  3,  3,  4,  5,  3,  5,  0, 99 };
const char enemy_type_defense_modifiers[ENEMY_TYPE_COUNT] = { 0, 1, 2, 2, 4,  3,  4,  5, 5,  6,  7,  8,  9, 13, 13, 13, 99, 99 };

#pragma data-name (push, "PROG0")
const char enemy_turn_rate[ENEMY_TYPE_COUNT] = { 128, 128, 171, 128, 171, 171, 192, 178, 128, 192, 192, 200, 192, 255, 64, 255, 128, 128};

const char enemy_config[ENEMY_TYPE_COUNT] = {
    /*Slime:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_DEFENSIVELY | ENEMY_FLAG_WANDERWALL,
    /*Beetle:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_DEFENSIVELY | ENEMY_FLAG_WANDERWALL,
    /*Goblin:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Zombie:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Ghost:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL | ENEMY_FLAG_HIT_AND_RUN,
    /*Skeleton:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_SOON | ENEMY_FLAG_WANDERWALL,
    /*Naga:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Ogre:*/ ENEMY_MOVEMENT_SIT | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Mage:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PASSIVE | ENEMY_FLAG_WANDERWALL | ENEMY_FLAG_HIT_AND_RUN | ENEMY_FLAG_RANGED_ATTACKS,
    /*Devil:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Automaton:*/ ENEMY_MOVEMENT_SIT | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Aberrant:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Snatcher:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PROXIMITY | ENEMY_FLAG_WANDERWALL,
    /*Manus Dexter:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_SOON | ENEMY_FLAG_HIT_AND_RUN,
    /*Remnant Fiend:*/ ENEMY_MOVEMENT_SIT | ENEMY_AGGRO_PASSIVE | ENEMY_FLAG_RANGED_ATTACKS,
    /*Manus Sinister:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_SOON | ENEMY_FLAG_HIT_AND_RUN,
    /*Kitty Cat:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PASSIVE | ENEMY_FLAG_CUTE,
    /*Big Kitty Cat:*/ ENEMY_MOVEMENT_WANDER | ENEMY_AGGRO_PASSIVE | ENEMY_FLAG_CUTE,
};

#pragma data-name (pop)

char enemy_closest_idx;
char enemy_closest_dist;
char action_counter;

#define ATTACK_TABLE_SIZE 16
const char enemy_attack_dmg_table[ATTACK_TABLE_SIZE] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3};

const char enemy_type_name[ENEMY_TYPE_COUNT] = {
    WORDS_TAG_SLIME_START,
    WORDS_TAG_BEETLE_START,
    WORDS_TAG_GOBLIN_START,
    WORDS_TAG_ZOMBIE_START,
    WORDS_TAG_GHOST_START,
    WORDS_TAG_SKELETON_START,
    WORDS_TAG_NAGA_START,
    WORDS_TAG_OGRE_START,
    WORDS_TAG_MAGE_START,
    WORDS_TAG_DEVIL_START,
    WORDS_TAG_AUTOMATON_START,
    WORDS_TAG_ABERRANT_START,
    WORDS_TAG_SNATCHER_START,
    WORDS_TAG_DEXTER_START,
    WORDS_TAG_FIEND_START,
    WORDS_TAG_SINISTER_START,
    WORDS_TAG_CAT_START,
    WORDS_TAG_CAT_START,
};

char enemy_idx;

extern char player_hp;

char roll_damage(signed char mod) {
    if(mod >= ATTACK_TABLE_SIZE) return 3;
    if(ATTACK_TABLE_SIZE-mod <= 0) return 0;
    if(mod > 0) 
        return enemy_attack_dmg_table[rnd_range(mod, ATTACK_TABLE_SIZE)];
    else
    return enemy_attack_dmg_table[rnd_range(0, ATTACK_TABLE_SIZE - mod)];
}

void reset_enemies() {
    action_counter = 0;
    enemy_count = 0;
    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        enemy_hp[enemy_idx] = 0;
    }
}

void damage_enemy(char enemy_id, char dmg) {
    if(enemy_data[enemy_id] & ENEMY_FLAG_CUTE) return;
    enemy_hp[enemy_id] -= dmg;
    if(enemy_hp[enemy_id] & 128) {
        enemy_hp[enemy_id] = 0;
    } else {
        if((enemy_data[enemy_id] & ENEMY_BITFIELD_AGGRO) == ENEMY_AGGRO_DEFENSIVELY) {
            enemy_data[enemy_id] &= ~ENEMY_BITFIELD_MOVEMENT;
            enemy_data[enemy_id] |= ENEMY_MOVEMENT_CHASE;
        }
    }
}

#pragma code-name (push, "PROG0")
char add_enemy(char type, char x, char y) {
    if(enemy_count == MAX_ENEMIES) {
        return 0;
    }

    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        if(enemy_hp[enemy_idx] == 0) {
            enemy_types[enemy_idx] = type;
            enemy_icons[enemy_idx] = 0x50 + type;
            enemy_hp[enemy_idx] = enemy_type_initial_hp[type];
            enemy_x[enemy_idx] = x;
            enemy_y[enemy_idx] = y;
            enemy_data[enemy_idx] = enemy_config[type];
            enemy_turn_counter[enemy_idx] = rnd_range(0, 256);
            ++enemy_count;


            if(type == 16) enemy_icons[enemy_idx] = 0x19;
            if(type == 17) enemy_icons[enemy_idx] = 0x1A;
            return enemy_idx+1;
        }
    }
    
    return 0;
}

extern char player_heal_tick;
void act_enemies_impl() {
    static signed char tx, ty, sx, sy;
    static char dmg, player_old_hp, attack_roll_counter, etype, skip, tctr;
    static int tmpidx;
    ++action_counter;

    tick_buff();

    enemy_closest_idx = 255;
    enemy_closest_dist = 255;
    for(enemy_idx = 0; enemy_idx < MAX_ENEMIES; ++enemy_idx) {
        if(enemy_hp[enemy_idx] != 0) {
            etype = enemy_types[enemy_idx];
            //If player is speed buffed, skip every other enemy turn
            //but still compute closest enemy
            tctr = enemy_turn_counter[enemy_idx] + enemy_turn_rate[etype];
            skip = tctr > enemy_turn_counter[enemy_idx];
            enemy_turn_counter[enemy_idx] = tctr;
            if(!skip && ((buff_type != BUFF_SPEED) || (action_counter & 1))) { 

                if((enemy_data[enemy_idx] & ENEMY_BITFIELD_MOVEMENT) == ENEMY_MOVEMENT_WANDER) {
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
                } else {
                    if(player_old_x > enemy_x[enemy_idx]) {
                        sx = 1;
                        tx = player_old_x - enemy_x[enemy_idx];
                    } else {
                        sx = -1;
                        tx = enemy_x[enemy_idx] - player_old_x;
                    }
                    
                    if(player_old_y > enemy_y[enemy_idx]){
                        sy = 1;
                        ty = player_old_y - enemy_y[enemy_idx];
                    } else {
                        sy = -1;
                        ty = enemy_y[enemy_idx] - player_old_y;
                    }

                    if((enemy_data[enemy_idx] & ENEMY_BITFIELD_MOVEMENT) == ENEMY_MOVEMENT_RETREAT) {
                        sx = -sx;
                        sy = -sy;
                    }

                    if(tx > ty) {
                        tx = enemy_x[enemy_idx] + sx;
                        ty = enemy_y[enemy_idx];
                    } else {
                        tx = enemy_x[enemy_idx];
                        ty = enemy_y[enemy_idx] + sy;
                    }
                }

                tmpidx = MAPINDEX(ty, tx);
                if(!(tilemap[tmpidx] & 128) && !(enemy_layer[tmpidx])) {
                    if((object_layer[tmpidx] & 0xF0) == 0x40) {
                        if(!(enemy_data[enemy_idx] & ENEMY_FLAG_CUTE)) {
                            dmg = roll_damage(enemy_type_attack_modifiers[etype]);
                            player_old_hp = player_hp;

                            attack_roll_counter = enemy_type_attack_modifiers[etype]+1;
                            while(attack_roll_counter--) {
                                player_hp -= dmg;
                            }

                            if(player_hp & 128) player_hp = 0;

                            if(dmg) {
                                if(buff_type == BUFF_GUARD) {
                                    player_hp = player_old_hp;
                                    push_log(WORDS_TAG_BLOCKED_START, enemy_type_name[etype], 255);
                                    play_sound_effect(ASSET__asset_main__spell_sfx_ID, 2);
                                    set_buff(BUFF_NONE);
                                } else {
                                    player_heal_tick = 0;
                                    flash_background();
                                    play_sound_effect(ASSET__asset_main__pain2_sfx_ID, 2);
                                    if(player_hp == 0) {
                                        dmg = WORDS_TAG_SLAIN_START;
                                    }
                                    push_log(WORDS_TAG_MISSED_START + dmg, WORDS_TAG_BY_START, enemy_type_name[etype]);
                                }
                            }

                            if(enemy_data[enemy_idx] & ENEMY_FLAG_HIT_AND_RUN) {
                                enemy_data[enemy_idx] &= ~ENEMY_BITFIELD_MOVEMENT;
                                enemy_data[enemy_idx] |= ENEMY_MOVEMENT_RETREAT;
                            }
                        }
                    } else {
                        if((enemy_data[enemy_idx] & ENEMY_BITFIELD_MOVEMENT) != ENEMY_MOVEMENT_SIT) {
                            enemy_layer[MAPINDEX(enemy_y[enemy_idx], enemy_x[enemy_idx])] = 0;
                            enemy_x[enemy_idx] = tx;
                            enemy_y[enemy_idx] = ty;
                            enemy_layer[tmpidx] = enemy_idx+1;
                        }
                    }
                } else {
                    //only switch to wander if hitting a wall, not another enemy
                    if(tilemap[tmpidx]&128) {
                        if(enemy_data[enemy_idx] & ENEMY_FLAG_WANDERWALL) {
                            enemy_data[enemy_idx] &= ~ENEMY_BITFIELD_MOVEMENT;
                            enemy_data[enemy_idx] |= ENEMY_MOVEMENT_WANDER;
                        }
                    }
                }
            }

            //reusing dmg as tmp distance var
            if(player_x > enemy_x[enemy_idx])
                tx = player_x - enemy_x[enemy_idx];
            else 
                tx = enemy_x[enemy_idx] - player_x;
            
            if(player_y > enemy_y[enemy_idx])
                ty = player_y - enemy_y[enemy_idx];
            else 
                ty = enemy_y[enemy_idx] - player_y;
            dmg = tx + ty;
            if(dmg < enemy_closest_dist) {
                enemy_closest_dist = dmg;
                enemy_closest_idx = enemy_idx;
            }

            switch(enemy_data[enemy_idx] & ENEMY_BITFIELD_AGGRO) {
                case ENEMY_AGGRO_PROXIMITY:
                if(dmg < ENEMY_PROXIMITY_AGGRO_RANGE) {
                    enemy_data[enemy_idx] &= ~ENEMY_BITFIELD_MOVEMENT;
                    enemy_data[enemy_idx] |= ENEMY_MOVEMENT_CHASE;
                }
                break;
                case ENEMY_AGGRO_SOON:
                if(rnd_range(0, 100) < 10) {
                    enemy_data[enemy_idx] &= ~ENEMY_BITFIELD_MOVEMENT;
                    enemy_data[enemy_idx] |= ENEMY_MOVEMENT_CHASE;
                }
                break;
            }
        }
    }
}
#pragma code-name (pop)

void act_enemies() {
    push_rom_bank();
    change_rom_bank(BANK_PROG0);
    act_enemies_impl();
    pop_rom_bank();
}