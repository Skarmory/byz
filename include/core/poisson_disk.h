#ifndef BYZ_CORE_POISSON_DISK_H
#define BYZ_CORE_POISSON_DISK_H

#include "core/list.h"

struct RNG;

struct List* poisson_disk(int width, int height, struct RNG* rng);

#endif
