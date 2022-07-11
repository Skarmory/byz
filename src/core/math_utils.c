#include "core/math_utils.h"

#include <math.h>

int clamp(int val, int low, int high)
{
    if(val < low) return low;
    if(val > high) return high;
    return val;
}

/**
 * Compute log of x to a given base
 */
float log_base(int x, int base)
{
    return log(x) / log(base);
}

float map_range(float value, float old_low, float old_high, float new_low, float new_high)
{
    float f = (value - old_low) / (old_high - old_low);

    return (f * (new_high - new_low)) + new_low;
}

// Transform (x, y) from coordinate system with origin (old_ox, old_oy) to new origin (new_ox, new_oy)
// e.g. old = (4, 7)
//      new = (8, 3)
//      in  = (2, 2)
//      output = (2 - 4 + 8, 2 - 7 + 3) = (6, -2)
void math_change_basis(const int x, const int y, const int old_ox, const int old_oy, const int new_ox, const int new_oy, int* x_out, int* y_out)
{
    *x_out = (x - old_ox) + new_ox;
    *y_out = (y - old_oy) + new_oy;
}
