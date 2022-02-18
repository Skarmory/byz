#ifndef BYZ_PATHING_F_H
#define BYZ_PATHING_F_H

#include "game/pathing_flags.h"

struct CONNECTIVITY_NODE;
struct List;
struct Map;
struct PATHING_GRID;

typedef float(*evaluation_func)(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* dest, PathingFlags pather_flags);

float creature_manhatten_evaluation(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* dest, PathingFlags pather_flags);

void pathing_find_path(
        struct CONNECTIVITY_NODE* start,
        struct CONNECTIVITY_NODE* end,
        PathingFlags pather_flags,
        evaluation_func eval_f,
        struct List* out_path
    );

#endif
