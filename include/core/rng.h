#ifndef BYZ_RANDOM_GENERATOR_H
#define BYZ_RANDOM_GENERATOR_H

struct RNG;

struct RNG* rng_new(int seed);
void        rng_free(struct RNG* rng);
int         rng_get(struct RNG* rng);

#endif
