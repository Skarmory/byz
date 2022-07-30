#include "game/pathing_node.h"

#include "core/list.h"

#include <stddef.h>

void pathing_data_init(struct PathingData* pathing_data)
{
    pathing_data->from            = NULL;
    pathing_data->to              = NULL;
    pathing_data->cost_to_end     = 0.0f;
    pathing_data->cost_from_start = 0.0f;
    pathing_data->state           = PATHING_NODE_STATE_UNVISITED;
    pathing_data->turn_visited    = -1;
    pathing_data->generation_id   = -1;
}

void connectivity_node_init(struct ConnectivityNode* node, int x, int y, float weight, PathingFlags pathing_flags)
{
    node->x = x;
    node->y = y;
    node->weight = weight;
    node->pathing_flags = pathing_flags;
    pathing_data_init(&node->pathing_data);
    node->connections = list_new();
}

void connectivity_node_add_connection(struct ConnectivityNode* node, struct ConnectivityNode* connection)
{
    list_add(node->connections, connection);
}

void connectivity_node_remove_connection(struct ConnectivityNode* node, struct ConnectivityNode* connection)
{
    list_rm(node->connections, list_find(node->connections, connection));
}
