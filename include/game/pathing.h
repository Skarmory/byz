#ifndef BYZ_PATHING_H
#define BYZ_PATHING_H

#include "game/pathing_flags.h"
#include "game/pathing_eval.h"

struct ConnectivityNode;
struct List;

void pathing_find_path(struct ConnectivityNode* start, struct ConnectivityNode* end, PathingFlags pather_flags, evaluation_func eval_f, struct List* out_path);

#endif
