#ifndef BYZ_PATHING_EVAL_H
#define BYZ_PATHING_EVAL_H

#include "game/pathing_flags.h"

struct ConnectivityNode;

typedef float(*evaluation_func)(struct ConnectivityNode* node, struct ConnectivityNode* dest, PathingFlags pather_flags);

float manhatten_evaluation(struct ConnectivityNode* node, struct ConnectivityNode* dest, PathingFlags pather_flags);

#endif
