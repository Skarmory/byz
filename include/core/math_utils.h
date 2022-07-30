#ifndef BYZ_MATH_UTILS_H
#define BYZ_MATH_UTILS_H

#define minu(a, b) ( (a) > (b) ? b : a )
#define maxu(a, b) ( (a) > (b) ? a : b )

int   clamp(int val, int low, int high);
float log_base(int x, int base);
float map_range(float value, float old_low, float old_high, float new_low, float new_high);

void math_change_basis(int x, int y, int old_ox, int old_oy, int new_ox, int new_oy, int* x_out, int* y_out);

#endif
