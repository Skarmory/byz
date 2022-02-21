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
