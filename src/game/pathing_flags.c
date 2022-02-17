#include "game/pathing_flags.h"

#include <string.h>

bool pathing_can_path(PathingFlags path_flags, PathingFlags pather_flags)
{
    return bit_flags_has_flags(path_flags, pather_flags);
}

enum PathingFlag pathing_path_flag_from_string(const char* name)
{
    if(strcmp(name, "none") == 0 || strcmp(name, "NONE") == 0)
    {
        return PATHING_NONE;
    }

    if(strcmp(name, "ground") == 0 || strcmp(name, "GROUND") == 0)
    {
        return PATHING_GROUND;
    }

    if(strcmp(name, "water") == 0 || strcmp(name, "WATER") == 0)
    {
        return PATHING_WATER;
    }

    if(strcmp(name, "flying") == 0 || strcmp(name, "FLYING") == 0)
    {
        return PATHING_FLYING;
    }

    return PATHING_NONE;
}

