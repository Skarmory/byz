#ifndef YUN_MATH_UTILS_H
#define YUN_MATH_UTILS_H

#define minu(a, b) ( (a) > (b) ? b : a )
#define maxu(a, b) ( (a) > (b) ? a : b )


int   clamp(int val, int low, int high);
float log_base(int x, int base);

#endif
