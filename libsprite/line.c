#include "allincludes.h"

static XSegment _lcache[NCOLORS][MAXCACHE];
static int _lcache_index[NCOLORS];

 void
FlushLineCache(win, color)
    Window  win;
    int     color;
{
    XDrawSegments(W_Display, win, colortable[color].contexts[0],
		  _lcache[color], _lcache_index[color]);
    _lcache_index[color] = 0;
}

/* for local window only */
void
W_CacheLine(window, X0, Y0, X1, Y1, color)
    W_Window window;
    int     X0, Y0, X1, Y1, color;
{
    Window  win = W_Void2Window(window)->drawable;
    register XSegment *s;

    if (_lcache_index[color] == MAXCACHE)
	FlushLineCache(win, color);

    s = &_lcache[color][_lcache_index[color]++];
    s->x1 = (short) X0;
    s->y1 = (short) Y0;
    s->x2 = (short) X1;
    s->y2 = (short) Y1;
}

void
W_FlushLineCaches(window)
    W_Window window;
{
    Window  win = W_Void2Window(window)->drawable;
    int i;
    for (i = 0; i < NCOLORS; i++) {
	if (_lcache_index[i])
	    FlushLineCache(win, i);
    }
}


void
W_MakeLine(window, X0, Y0, X1, Y1, color)
    W_Window window;
    int     X0, Y0, X1, Y1;
    W_Color color;
{
    Window  win;

#ifdef DEBUG
    printf("Line on %d\n", window);
#endif
    win = W_Void2Window(window)->drawable;
    XDrawLine(W_Display, win, colortable[color].contexts[0], X0, Y0, X1, Y1);
}

void
W_MakeTractLine(window, X0, Y0, X1, Y1, color)
    W_Window window;
    int     X0, Y0, X1, Y1;
    W_Color color;
{
    Window  win;

#ifdef DEBUG
    printf("Line on %d\n", window);
#endif
    win = W_Void2Window(window)->drawable;
    XDrawLine(W_Display, win, colortable[color].contexts[3], X0, Y0, X1, Y1);
}

