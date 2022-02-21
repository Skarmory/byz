#ifndef BYZ_PATHING_EVAL_H
#define BYZ_PATHING_EVAL_H

#include "game/pathing_flags.h"

struct CONNECTIVITY_NODE;

typedef float(*evaluation_func)(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* dest, PathingFlags pather_flags);

float manhatten_evaluation(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* dest, PathingFlags pather_flags);

#endif
