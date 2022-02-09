#ifndef BYZ_UI_BOX_H
#define BYZ_UI_BOX_H

#include <stdbool.h>

struct UIBox
{
    int x;
    int y;
    int w;
    int h;
};

struct UIBox* ui_box_new(int x, int y, int w, int h);
void          ui_box_free(struct UIBox* ui_box);

void          ui_box_draw(struct UIBox* box, bool invalidate_inner_area);

#endif
