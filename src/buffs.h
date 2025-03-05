#ifndef BUFFS_H
#define BUFFS_H

#define BUFF_INITIAL_TIME 32

#define BUFF_NONE 0
#define BUFF_SPEED 1
#define BUFF_STRENGTH 2
#define BUFF_GUARD 3
#define BUFF_TYPE_COUNT 4
extern char buff_duration;
extern char buff_type;
extern const char buff_names[BUFF_TYPE_COUNT];

void set_buff(char newbuff);

void tick_buff();

#endif