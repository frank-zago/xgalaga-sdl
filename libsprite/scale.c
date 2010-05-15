#include "allincludes.h"

void
W_TranslatePoints(window, x, y)
    int    *x, *y;
    W_Window window;
{
    struct window *win;
    win = W_Void2Window(window);

    if (win->type == WIN_TEXT) {
	*y = (*y - MENU_PAD) / W_Textheight;
	*x = (*x - MENU_PAD) / W_Textwidth;
    }
    return;
}

