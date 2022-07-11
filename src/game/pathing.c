#include "game/pathing.h"

#include "core/list.h"
#include "core/log.h"

#include "game/globals.h"
#include "game/pathing_node.h"
#include "game/movement.h"

#include <stddef.h>

// TODO: Implement a sorted linked list?

static int         generation_id = 0;
static struct List open_list;

static void _add_open_node(struct CONNECTIVITY_NODE* node)
{
#ifdef DEBUG_PATHING
    log_format_msg(LOG_DEBUG, "Adding to open list: (%d, %d)", node->x, node->y);
    log_push_indent();
#endif

    node->pathing_data.state = PATHING_NODE_STATE_OPEN;

    struct CONNECTIVITY_NODE* comp = list_peek_head(&open_list);
    if(!comp)
    {
#ifdef DEBUG_PATHING
        log_format_msg(LOG_DEBUG, "Open list empty");
        log_pop_indent();
#endif

        list_add(&open_list, node);
        return;
    }

    if(node->pathing_data.cost_to_end < comp->pathing_data.cost_to_end)
    {
#ifdef DEBUG_PATHING
        log_format_msg(LOG_DEBUG, "Node more efficient than head, setting to open list head");
#endif

        // Try to keep the best node at head of the list for easy retrieval
        list_add_head(&open_list, node);
    }
    else
    {
#ifdef DEBUG_PATHING
        log_format_msg(LOG_DEBUG, "Searching open list to add node");
#endif

        struct ListNode* list_node = NULL;
        list_for_each(&open_list, list_node)
        {
            comp = list_node->data;

            if(node->pathing_data.cost_to_end < comp->pathing_data.cost_to_end)
            {
#ifdef DEBUG_PATHING
                log_format_msg(LOG_DEBUG, "Inserting into list");
                log_pop_indent();
#endif

                list_insert_before(&open_list, node, list_node);
                return;
            }
        }

#ifdef DEBUG_PATHING
        log_msg(LOG_DEBUG, "Adding to end of list");
#endif

        list_add(&open_list, node);
    }

#ifdef DEBUG_PATHING
    log_pop_indent();
#endif
}

static void _reset_node(struct CONNECTIVITY_NODE* node)
{
    node->pathing_data.from          = NULL;
    node->pathing_data.to            = NULL;
    node->pathing_data.turn_visited  = current_turn_number;
    node->pathing_data.generation_id = generation_id;
    node->pathing_data.state         = PATHING_NODE_STATE_UNVISITED;
}

static struct CONNECTIVITY_NODE* _find_path(struct CONNECTIVITY_NODE* dest, PathingFlags pather_flags, evaluation_func eval_f)
{
    const int iterations_max = 1000;
    int iterations = 0;

    struct CONNECTIVITY_NODE* best_node = NULL;

    while(!list_empty(&open_list) && iterations < iterations_max)
    {
        best_node = list_pop_head(&open_list);
        best_node->pathing_data.state = PATHING_NODE_STATE_CLOSED;

        // Reached destination, return
        if(best_node->x == dest->x && best_node->y == dest->y)
        {
            break;
        }

        struct ListNode* connection_list_node = NULL;
        list_for_each(best_node->connections, connection_list_node)
        {
            struct CONNECTIVITY_NODE* node = connection_list_node->data;

            // Check to see if this is a stale node (if it was last visited in a previous turn or a previous pathing request this turn)
            // It it technically possible for a stale node clash to occur, if we do INT_MAX worth of requests in a single turn (unlikely)
            // Set it to a fresh state
            if(node->pathing_data.turn_visited != current_turn_number || node->pathing_data.generation_id != best_node->pathing_data.generation_id)
            {
                _reset_node(node);
            }

            if(node->pathing_data.state == PATHING_NODE_STATE_CLOSED)
            {
                // Check if this new path is more cost-efficient
                // If it is, we can reassign the path to this closed node
                if(node->pathing_data.cost_from_start > best_node->pathing_data.cost_from_start + 1)
                {
                    node->pathing_data.state           = PATHING_NODE_STATE_OPEN;
                    node->pathing_data.from            = best_node;
                    node->pathing_data.cost_from_start = best_node->pathing_data.cost_from_start + 1;
                }
            }
            else if(node->pathing_data.state == PATHING_NODE_STATE_UNVISITED)
            {
                // This location has not been visited yet
                // Add to open list
                node->pathing_data.to   = NULL;
                node->pathing_data.from = best_node;

                node->pathing_data.cost_from_start = best_node->pathing_data.cost_from_start + 1;
                node->pathing_data.cost_to_end     = eval_f(node, dest, pather_flags);

                _add_open_node(node);
            }
        }

#if DEBUG_PATHING
        _log_open_list();
#endif

        ++iterations;
    }

    return best_node;
}

static void _make_path_list(struct CONNECTIVITY_NODE* node, struct List* out_path)
{
    while(node)
    {
        list_add_head(out_path, node);
        node = node->pathing_data.from;
    }
}

#ifdef DEBUG_PATHING
static void _log_open_list(void)
{
    log_msg(LOG_DEBUG, "Open list:");
    log_push_indent();

    struct ListNode* node = NULL;
    list_for_each(&open_list, node)
    {
        struct CONNECTIVITY_NODE* conn = node->data;

        log_format_msg(LOG_DEBUG, "(%d, %d)", conn->x, conn->y);
    }

    log_pop_indent();
}

static void _log_path(struct CONNECTIVITY_NODE* node)
{
    log_push_indent();

    do
    {
        log_format_msg(LOG_DEBUG, "(%d, %d)", node->x, node->y);
        node = node->pathing_data.to;
    }
    while(node->pathing_data.to);

    log_pop_indent();
}
#endif

void pathing_find_path(struct CONNECTIVITY_NODE* restrict start, struct CONNECTIVITY_NODE* restrict end, PathingFlags pather_flags, evaluation_func eval_f, struct List* out_path)
{
    ++generation_id;

#if DEBUG_PATHING
    log_msg(LOG_DEBUG, "pathing_find_path()");
    log_format_msg(LOG_DEBUG, "(%d, %d) -> (%d, %d), turn: %d, generation: %d", start->x, start->y, end->x, end->y, current_turn_number, generation_id);
    log_push_indent();
#endif

    list_uninit(&open_list);

    // Initialis starting node and add to open list
    _reset_node(start);
    start->pathing_data.cost_from_start = 0.0f;
    start->pathing_data.cost_to_end     = eval_f(start, end, pather_flags);
    _add_open_node(start);

    // This is as far as the path goes, maybe it's the destination, or as close as we can get
    struct CONNECTIVITY_NODE* path_end = _find_path(end, pather_flags, eval_f);
    _make_path_list(path_end, out_path);

#if DEBUG_PATHING
    log_pop_indent();
#endif
}
