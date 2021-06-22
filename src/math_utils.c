#include "math_utils.h"

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
