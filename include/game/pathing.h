#ifndef BYZ_PATHING_H
#define BYZ_PATHING_H

#include "game/pathing_flags.h"
#include "game/pathing_eval.h"

struct CONNECTIVITY_NODE;
struct List;
struct PATHING_GRID;

void pathing_find_path(struct CONNECTIVITY_NODE* start, struct CONNECTIVITY_NODE* end, PathingFlags pather_flags, evaluation_func eval_f, struct List* out_path);

#endif
