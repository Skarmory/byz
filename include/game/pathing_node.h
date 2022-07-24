#ifndef BYZ_PATHING_NODE_H
#define BYZ_PATHING_NODE_H

#include "game/pathing_flags.h"

enum PathingNodeState
{
    PATHING_NODE_STATE_UNVISITED = 0,
    PATHING_NODE_STATE_OPEN = 1,
    PATHING_NODE_STATE_CLOSED = 2
};

/* Internal pathfinding data
 * Each pathfind request uses these to store data whilt the algorithm runs
 */
struct PathingData
{
    struct ConnectivityNode* from;
    struct ConnectivityNode* to;
    float                    cost_to_end;
    float                    cost_from_start;
    enum PathingNodeState    state;

    int                      turn_visited;
    int                      generation_id;
};

void pathing_data_init(struct PathingData* per_path_data);

/* Node containing location and game data for pathing
 */
struct ConnectivityNode
{
    int                x;
    int                y;
    float              weight;
    PathingFlags       pathing_flags;
    struct PathingData pathing_data;
    struct List*       connections;
};

void connectivity_node_init(struct ConnectivityNode* node, int x, int y, float weight, PathingFlags pathing_flags);
void connectivity_node_add_connection(struct ConnectivityNode* node, struct ConnectivityNode* connection);
void connectivity_node_remove_connection(struct ConnectivityNode* node, struct ConnectivityNode* connection);

#endif
