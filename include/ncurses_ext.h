#ifndef NCURSES_EXT
#define NCURSES_EXT
#include <ncurses.h>

void mvprintw_xy(int x, int y, char* str, ...);
void mvprintwa(int x, int y, int attr, char* str, ...);

#endif
