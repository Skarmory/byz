#include "game/pathing_eval.h"

#include "game/pathing_node.h"

#include <math.h>

float manhatten_evaluation(struct ConnectivityNode* node, struct ConnectivityNode* dest, PathingFlags pather_flags)
{
    // Special handling for the destination
    if(node->x == dest->x && node->y == dest->y)
    {
        return -INFINITY;
    }

    // Check to see whether the pather can use this location
    if(!pathing_can_path(node->pathing_flags, pather_flags))
    {
        // Cannot path to this node, so make it ridicuously high evaluation
        return INFINITY;
    }

    // Use distance squared to avoid sqrt
    float _x = dest->x - node->x;
    float _y = dest->y - node->y;
    float dist2 = _x * _x + _y * _y;

    return dist2 + node->weight;
}
