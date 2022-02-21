#include "core/rng.h"

#include <stdlib.h>

#define C_N 624

const int c_w        = 32;
const int c_r        = c_w - 1;
const int c_m        = 397;
const int c_a        = 0x9908b0df;
const int c_u        = 11;
const int c_d        = 0xffffffff;
const int c_s        = 7;
const int c_b        = 0x9d2c5680;
const int c_t        = 15;
const int c_c        = 0xefc60000;
const int c_l        = 18;
const int lower_mask = (1 << c_r) - 1;
const int upper_mask = ~lower_mask;

const int c_f        = 0x9b73cf98;

struct RNG
{
    int mt[C_N];
    int index;
};

static void _twist(struct RNG* rng)
{
    for(int i = 0; i < C_N; ++i)
    {
        int x = (rng->mt[i] & upper_mask) + (rng->mt[i+1 % C_N] & lower_mask);
        int xA = x >> 1;

        if((x % 2) != 0)
        {
            xA ^= c_a;
        }

        rng->mt[i] = rng->mt[(i + c_m ) % C_N] ^ xA;
    }

    rng->index = 0;
}

struct RNG* rng_new(int seed)
{
    struct RNG* rng = malloc(sizeof(struct RNG));

    rng->index = C_N;
    rng->mt[0] = seed;

    for(int i = 1; i < C_N; ++i)
    {
        int test = (c_f * (rng->mt[i-1] ^ (rng->mt[i-1] >> (c_w - 2))) + i);
        rng->mt[i] = lower_mask & test;
    }

    return rng;
}

void rng_free(struct RNG* rng)
{
    free(rng);
}

int rng_get(struct RNG* rng)
{
    if(rng->index >= C_N)
    {
        _twist(rng);
    }

    int value = rng->mt[rng->index];
    value ^= ((value >> c_u) & c_d);
    value ^= ((value << c_s) & c_b);
    value ^= ((value << c_t) & c_c);
    value ^= (value >> c_l);

    ++rng->index;

    return lower_mask & value;
}
