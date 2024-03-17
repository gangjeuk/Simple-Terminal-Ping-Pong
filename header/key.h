#pragma once
#include <sys/select.h>
#define KEY_ESC     27
#define KEY_ENTER   13
#define KEY_LEFT    1
#define KEY_MULTIPLAY 109
#define KEY_CONNECT 99

// Functions to set stdin to be character based so I can catch inputs
void reset_terminal_mode();
void set_conio_terminal_mode();
int keyboard_hit(int fs);
int get_char(int fs);
