#ifndef LOGTEXT_H
#define LOGTEXT_H

#define LOG_DIGIT_ANTISPACE 254

extern char log_text_bg;

void prepare_log_text();

void push_log(char partA, char partB, char partC);
void push_log_num(char partA, char partB, char partC);

void show_logs(char x, char y, char count);

#endif