#include "ui_box.h"

#include "colour.h"

#include <stdlib.h>

struct UIBox* ui_box_new(int x, int y, int w, int h)
{
    struct UIBox* ui_box = malloc(sizeof(struct UIBox));

    ui_box->x = x;
    ui_box->y = y;
    ui_box->w = w;
    ui_box->h = h;

    return ui_box;
}

void ui_box_free(struct UIBox* ui_box)
{
    free(ui_box);
}


void ui_box_draw(struct UIBox* box, bool invalidate_inner_area)
{
    // Draw borders
    for(int x = box->x + 1; x < box->w; ++x)
    {
        term_draw_symbol(x, box->y,          COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '-');
        term_draw_symbol(x, box->y + box->h, COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '-');
    }

    for(int y = box->y + 1; y < box->h; ++y)
    {
        term_draw_symbol(box->x,          y, COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '|');
        term_draw_symbol(box->x + box->w, y, COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '|');
    }

    // Corners
    {
        term_draw_symbol(box->x,          box->y,          COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '+');
        term_draw_symbol(box->x + box->w, box->y,          COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '+');
        term_draw_symbol(box->x,          box->y + box->h, COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '+');
        term_draw_symbol(box->x + box->w, box->y + box->h, COL(CLR_WHITE), COL(CLR_DEFAULT), 0, '+');
    }

    if(invalidate_inner_area)
    {
        term_clear_area(box->x + 1, box->y + 1, box->w - 1, box->h - 1);
    }
}
