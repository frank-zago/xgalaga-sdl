#include "allincludes.h"

void
W_WriteAnyTriangle(window, X1, Y1, X2, Y2, X3, Y3, color)
    W_Window window;
    int     X1, Y1, X2, Y2, X3, Y3;
    W_Color color;
{
    struct window *win = W_Void2Window(window);
    XPoint  points[3];

    points[0].x = X1;
    points[0].y = Y1;
    points[1].x = X2;
    points[1].y = Y2;
    points[2].x = X3;
    points[2].y = Y3;

    XFillPolygon(W_Display, win->drawable, colortable[color].contexts[0],
		 points, 3, Convex, CoordModeOrigin);
}

void
W_WriteTriangle(window, x, y, s, t, color)
    W_Window window;
    int     x, y, s;
    int     t;
    W_Color color;
{
    struct window *win = W_Void2Window(window);
    XPoint  points[3];

    if (t == 0) {
	points[0].x = x;
	points[0].y = y;
	points[1].x = x + s;
	points[1].y = y - s;
	points[2].x = x - s;
	points[2].y = y - s;
    } else {
	points[0].x = x;
	points[0].y = y;
	points[1].x = x + s;
	points[1].y = y + s;
	points[2].x = x - s;
	points[2].y = y + s;
    }


    XFillPolygon(W_Display, win->drawable, colortable[color].contexts[0],
		 points, 3, Convex, CoordModeOrigin);
}

