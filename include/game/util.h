#ifndef BYZ_UTIL_H
#define BYZ_UTIL_H

#include <stddef.h>

void convert_arg(char c, char* buf);

void do_quit(void);

void sigint_handler(int _);

int random_int(int hi, int lo);

int roll_dice(int dice_count, int dice_sides);

int roll_d100(void);

float roll_d100f(void);

#endif
