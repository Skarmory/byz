#include "game/embark.h"

#include "core/list.h"
#include "core/math_utils.h"
#include "core/tasking.h"

#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_gen.h"

#include "ui/embark_screen.h"

enum EmbarkCommand
{
    CURSOR_LEFT  = KEYCODE_h,
    CURSOR_RIGHT = KEYCODE_l,
    CURSOR_DOWN  = KEYCODE_j,
    CURSOR_UP    = KEYCODE_k,
    INVALID
};

struct Embark
{
    struct Map* map;
    struct List gen_tasks;
    struct EmbarkScreen* ui;
    struct camera world_camera;
    struct camera regional_camera;
};

static void _await_load_tasks(struct List* gen_tasks_list)
{
    struct ListNode* n = NULL;
    list_for_each(gen_tasks_list, n)
    {
        task_await((struct Task*)n->data);
    }

    list_free_data(gen_tasks_list, &task_free_wrapper);
}

static struct List _load_cells_around_region(struct Map* map, int rx, int ry)
{
    struct List new_tasks;
    list_init(&new_tasks);

    for(int x = (rx - 5); x < (rx + 5); ++x)
    for(int y = (ry - 5); y < (ry + 5); ++y)
    {
        struct MapCell* cell = map_get_cell_by_cell_coord(map, x, y);
        if(cell && cell->load_state == MAP_CELL_UNLOADED)
        {
            list_add(&new_tasks, gen_map_cell_async(map, cell));
        }
    }

    return new_tasks;
}

static void _cursor_to_world(struct camera* camera, struct Cursor* c, struct Container* ct, int* wx, int* wy)
{
    math_change_basis(c->x, c->y, ct->x, ct->y, 0, 0, wx, wy);
    camera_relative_to_world(camera, *wx, *wy, wx, wy);
}

static void _update_camera(struct Embark* e, int ox, int oy, enum EmbarkCommand cmd)
{
    struct Container ct = embark_screen_get_container(e->ui);
    struct Cursor c = embark_screen_get_cursor(e->ui);

    switch(cmd)
    {
        case CURSOR_LEFT:
        {
            if(!(c.x >= ct.x && c.x < ct.x + ct.w / 4))
            {
                return;
            }

            break;
        }

        case CURSOR_RIGHT:
        {
            if(!(c.x >= ct.x + ct.w - ct.w / 4 && c.x < ct.w))
            {
                return;
            }

            break;
        }

        case CURSOR_UP:
        {
            if(!(c.y >= ct.y && c.y < ct.y + ct.h / 4))
            {
                return;
            }

            break;
        }

        case CURSOR_DOWN:
        {
            if(!(c.y >= ct.y + ct.h - ct.h / 4 && c.y < ct.h))
            {
                return;
            }

            break;
        }
    }

    struct camera* cam = embark_screen_world_map_focussed(e->ui) ? &e->world_camera : &e->regional_camera;
    if(map_in_bounds(e->map, cam->x + ox, cam->y + oy) && map_in_bounds(e->map, camera_max_x(cam) + ox, camera_max_y(cam) + oy))
    {
        cam->x += ox;
        cam->y += oy;
    }
}

static struct MapCell* _current_world_map_cell(struct Embark* e)
{
    struct Container wc = embark_screen_get_world_map_container(e->ui);
    struct Cursor c = embark_screen_get_world_map_container_cursor(e->ui);

    int wx = -1;
    int wy = -1;

    _cursor_to_world(&e->world_camera, &c, &wc, &wx, &wy);

    return map_get_cell_by_cell_coord(e->map, wx, wy);
}

static struct MapLocation* _current_regional_map_loc(struct Embark* e)
{
    int wx = -1;
    int wy = -1;

    struct MapCell* cell = _current_world_map_cell(e);

    struct Container rc = embark_screen_get_regional_map_container(e->ui);
    struct Cursor c = embark_screen_get_regional_map_container_cursor(e->ui);

    _cursor_to_world(&e->regional_camera, &c, &rc, &wx, &wy);

    return map_cell_get_location_relative(cell, wx, wy);
}

struct Embark* embark_new(struct Map* map)
{
    struct Embark* e = malloc(sizeof(struct Embark));
    e->map = map;
    e->ui = embark_screen_new();
    list_init(&e->gen_tasks);

    struct Container wc = embark_screen_get_world_map_container(e->ui);
    struct Container rc = embark_screen_get_regional_map_container(e->ui);

    // Create cameras to display world and regional map views
    e->world_camera.x = 0;
    e->world_camera.y = 0;
    e->world_camera.w = minu(wc.w, map->width);
    e->world_camera.h = minu(wc.h, map->height);

    e->regional_camera.x = 0;
    e->regional_camera.y = 0;
    e->regional_camera.w = minu(rc.w, g_map_cell_width);
    e->regional_camera.h = minu(rc.h, g_map_cell_height);

    struct MapCell* cell = _current_world_map_cell(e);
    struct List new_tasks = _load_cells_around_region(e->map, cell->cell_x, cell->cell_y);
    list_splice(&new_tasks, &e->gen_tasks, 0, maxu(e->gen_tasks.count-1, 0), new_tasks.count);

    return e;
}

void embark_free(struct Embark* e)
{
    list_free_data(&e->gen_tasks, &task_free_wrapper);
    embark_screen_free(e->ui);
    free(e);
}

const struct Map* embark_get_map(struct Embark* e)
{
    return e->map;
}

bool embark_update(struct Embark* e, enum KeyCode input)
{
    bool updated = embark_screen_update(e->ui, input);

    enum EmbarkCommand cmd = (enum EmbarkCommand)input;

    switch(cmd)
    {
        case CURSOR_LEFT:
        case CURSOR_RIGHT:
        case CURSOR_DOWN:
        case CURSOR_UP:
        {
            int ox = -1;
            int oy = -1;
            cursor_get_offset(input, &ox, &oy);

            if(embark_screen_world_map_focussed(e->ui))
            {
                struct Cursor cursor = embark_screen_get_cursor(e->ui);
                struct Container container = embark_screen_get_container(e->ui);

                int cx = -1;
                int cy = -1;
                _cursor_to_world(&e->world_camera, &cursor, &container, &cx, &cy);

                struct List new_tasks = _load_cells_around_region(e->map, cx, cy);
                list_splice(&new_tasks, &e->gen_tasks, 0, maxu(e->gen_tasks.count-1, 0), new_tasks.count);
            }

            _update_camera(e, ox, oy, cmd);

            break;
        }
    }

    return updated;
}

void embark_draw(struct Embark* e)
{
    struct MapCell* current_cell = _current_world_map_cell(e);
    if(!current_cell || current_cell->load_state != MAP_CELL_LOADED)
    {
        _await_load_tasks(&e->gen_tasks);
    }

    void* info_data = embark_screen_world_map_focussed(e->ui) ? (void*)current_cell : (void*)_current_regional_map_loc(e);
    embark_screen_draw_world_map(e->ui, e->map, &e->world_camera);
    embark_screen_draw_regional_map(e->ui, current_cell, &e->regional_camera);
    embark_screen_draw_info_view(e->ui, info_data);
}
