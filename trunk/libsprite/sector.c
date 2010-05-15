#include "allincludes.h"

void
W_DrawSectorHighlight(window, x, y, width, h, color)
    W_Window window;
    int     x, y, width, h;
    W_Color color;
{
    Window  win;
#ifdef YUCK
    XRectangle r[2];

    r[0].x = (short) x;
    r[0].y = (short) y;
    r[0].width = (unsigned short) width;
    r[0].height = (unsigned short) h;
    r[1].x = (short) x + 2;
    r[1].y = (short) y + 2;
    r[1].width = (unsigned short) width - 4;
    r[1].height = (unsigned short) h - 4;

    win = W_Void2Window(window)->drawable;
    XDrawRectangles(W_Display, win, colortable[color].contexts[3],
		    r, 2);
#else
    XRectangle r[1];

    r[0].x = (short) x + 2;
    r[0].y = (short) y + 2;
    r[0].width = (unsigned short) width - 4;
    r[0].height = (unsigned short) h - 4;

    win = W_Void2Window(window)->drawable;
    XDrawRectangles(W_Display, win, colortable[color].contexts[3],
		    r, 1);
#endif
}
