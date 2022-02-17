#ifndef BYZ_PATHING_FLAGS_H
#define BYZ_PATHING_FLAGS_H

#include "core/bit_flags.h"

#include <stdbool.h>

enum PathingFlag
{
    PATHING_NONE   = 0u,
    PATHING_GROUND = BIT_FLAG(1),
    PATHING_WATER  = BIT_FLAG(2),
    PATHING_FLYING = BIT_FLAG(3)
};
typedef unsigned int PathingFlags;

bool pathing_can_path(PathingFlags path_flags, PathingFlags pather_flags);

enum PathingFlag pathing_path_flag_from_string(const char* name);

#endif
