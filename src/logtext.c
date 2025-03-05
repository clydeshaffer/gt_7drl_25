#include "logtext.h"
#include "gt/gfx/draw_queue.h"
#include "gt/banking.h"
#include "gen/assets/asset_main.h"
#include "gen/assets/asset_main/words.json.h"


SpriteSlot text_sheet;

#define LOG_BUF_SIZE 32
#define LOG_ENTRY_SIZE 4
//must be LOG_BUF_SIZE / LOG_ENTRY_SIZE
#define LOG_MAX_LINES 8
#define LOG_LINE_HEIGHT 8
char log_tail;
char log_head;
char log_count;
char log_dirty;
char log_chunk_buf[LOG_BUF_SIZE];
char last_log_count;

void prepare_log_text() {
    text_sheet = allocate_sprite(&ASSET__asset_main__words_bmp_load_list);
    set_sprite_frametable(text_sheet, ASSET__asset_main__words_json);
    log_count = 0;
    log_tail = 0;
    log_head = 0;
    log_dirty = 2;
    last_log_count = 0;
}

void log_increment_queue() {
    if(log_tail >= LOG_BUF_SIZE) log_tail -= LOG_BUF_SIZE;
    if(log_count < LOG_MAX_LINES) {
        ++log_count;
    } else {
        log_head+=4;
        if(log_head >= LOG_BUF_SIZE) log_head -= LOG_BUF_SIZE;
    }
    log_dirty = 2;
}

void push_log_num(char partA, char partB, char partC) {
    log_chunk_buf[log_tail++] = partA;
    log_chunk_buf[log_tail++] = partB;
    log_chunk_buf[log_tail++] = LOG_DIGIT_ANTISPACE;
    log_chunk_buf[log_tail++] = partC;

    log_increment_queue();
}

void push_log(char partA, char partB, char partC) {
    log_chunk_buf[log_tail++] = partA;
    log_chunk_buf[log_tail++] = partB;
    log_chunk_buf[log_tail++] = partC;
    log_chunk_buf[log_tail++] = 255;
    
    log_increment_queue();
}

void show_logs(char x, char y, char count) {
    static char i, tok, tx, ty;
    if(count != last_log_count) log_dirty = 2;
    last_log_count = count;
    if(!log_dirty) return;
    --log_dirty;
    ty = y;
    i = log_tail - 4;
    while(i & 128) i += LOG_BUF_SIZE;
    push_rom_bank();
    change_rom_bank(ASSET__asset_main__words_json_bank);
    queue_draw_box(x, y, 127, count << 3, 0);
    if(count > log_count) count = log_count;
    while(count > 0) {
        tx = x;
        for(tok = 0; tok < 4; ++tok) {
            if(log_chunk_buf[i] < WORDS_SHEET_FRAMES) {
                queue_draw_sprite_frame(text_sheet, tx, ty, log_chunk_buf[i], 0);
                tx += sprite_temp_frame.w + 4;
            }
            if(log_chunk_buf[i] == LOG_DIGIT_ANTISPACE) {
                tx -= 3;
            }
            ++i;
        }
        ty += LOG_LINE_HEIGHT;
        i -= 8;
        if(i & 128) i += LOG_BUF_SIZE;
        --count;
    }
    pop_rom_bank();
}