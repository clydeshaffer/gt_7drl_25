#include "buffs.h"
#include "gen/assets/asset_main/words.json.h"
#include "logtext.h"

const char buff_names[BUFF_TYPE_COUNT] = {
    255,
    WORDS_TAG_QUICK_START,
    WORDS_TAG_STRONG_START,
    WORDS_TAG_PROTECTED_START
};
char buff_duration = 0;
char buff_type = 0;

void set_buff(char newbuff) {
    if(buff_type) {
        push_log(WORDS_TAG_YOU_NO_LONGER_FEEL_START, buff_names[buff_type], 255);
    }
    buff_type = newbuff;
    if(buff_type) {
        buff_duration = BUFF_INITIAL_TIME;
        push_log(WORDS_TAG_YOU_FEEL_START, buff_names[buff_type], 255);
    } else {
        buff_duration = 0;
    }
}

void tick_buff() {
    if(buff_duration) {
        if(buff_type != BUFF_GUARD) {
            --buff_duration;
            if(!buff_duration) {
                set_buff(BUFF_NONE);
            }
        }
    }
}