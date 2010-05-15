#include "allincludes.h"

void
W_FillArea(window, x, y, width, height, color)
    W_Window window;
    int     x, y;
    unsigned int width, height;
    W_Color color;
{
    struct window *win;

#ifdef DEBUG
    printf("Clearing (%d %d) x (%d %d) with %d on %d\n", x, y, width, height,
	   color, window);
#endif
    win = W_Void2Window(window);
    switch (win->type) {
    case WIN_GRAPH:
	XFillRectangle(W_Display, win->drawable, colortable[color].contexts[0],
		       x, y, width, height);
	break;
    default:
	XFillRectangle(W_Display, win->drawable, colortable[color].contexts[0],
		    WIN_EDGE + x * W_Textwidth, MENU_PAD + y * W_Textheight,
		       width * W_Textwidth, height * W_Textheight);
    }
}

