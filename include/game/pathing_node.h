#ifndef BYZ_PATHING_NODE_H
#define BYZ_PATHING_NODE_H

#include "game/pathing_flags.h"

enum PATHING_NODE_STATE
{
    PATHING_NODE_STATE_UNVISITED = 0,
    PATHING_NODE_STATE_OPEN = 1,
    PATHING_NODE_STATE_CLOSED = 2
};

/* Internal pathfinding data
 * Each pathfind request uses these to store data whilt the algorithm runs
 */
struct PATHING_DATA
{
    struct CONNECTIVITY_NODE* from;
    struct CONNECTIVITY_NODE* to;
    float                     cost_to_end;
    float                     cost_from_start;
    enum PATHING_NODE_STATE   state;

    int                       turn_visited;
    int                       generation_id;
};

void pathing_data_init(struct PATHING_DATA* per_path_data);

/* Node containing location and game data for pathing
 */
struct CONNECTIVITY_NODE
{
    int                 x;
    int                 y;
    float               weight;
    PathingFlags        pathing_flags;
    struct PATHING_DATA pathing_data;
    struct List*        connections;
};

void connectivity_node_init(struct CONNECTIVITY_NODE* node, int x, int y, float weight, PathingFlags pathing_flags);
void connectivity_node_add_connection(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* connection);
void connectivity_node_remove_connection(struct CONNECTIVITY_NODE* node, struct CONNECTIVITY_NODE* connection);

#endif
