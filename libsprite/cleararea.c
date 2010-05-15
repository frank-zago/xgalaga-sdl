#include "allincludes.h"

/* XFIX */

static XRectangle _rcache[MAXCACHE];
static int _rcache_index;

 void
FlushClearAreaCache(window)
    W_Window  window;
{
    struct window *win = W_Void2Window(window);
    XFillRectangles(W_Display, win->drawable, colortable[backColor].contexts[0],
		    _rcache, _rcache_index);
    _rcache_index = 0;
}

/* local window only */
void
W_CacheClearArea(window, x, y, width, height)
    W_Window window;
    int     x, y, width, height;
{
    register XRectangle *r;

    if (_rcache_index == MAXCACHE)
	FlushClearAreaCache(window);

    r = &_rcache[_rcache_index++];
    r->x = (short) x;
    r->y = (short) y;
    r->width = (unsigned short) width;
    r->height = (unsigned short) height;
}

void
W_FlushClearAreaCache(window)
    W_Window window;
{
    if (_rcache_index)
	FlushClearAreaCache(window);
}

/* XFIX: clears now instead of filling. */
/* not any more.  Can't clear a drawable. [BDyess] */
void
W_ClearArea(window, x, y, width, height)
    W_Window window;
    int     x, y;
    unsigned int width, height;
{
    struct window *win;

#ifdef DEBUG
    printf("Clearing (%d %d) x (%d %d) with %d on %d\n", x, y, width, height,
	   color, window);
#endif
    win = W_Void2Window(window);
    switch (win->type) {
    case WIN_GRAPH:
	/* XFIX: changed */
	XFillRectangle(W_Display, win->drawable, 
	    colortable[backColor].contexts[0], x, y, width, height);
	break;
    default:
	/* XFIX: changed */
	XFillRectangle(W_Display, win->drawable, 
	   colortable[backColor].contexts[0], WIN_EDGE + x * W_Textwidth,
	   MENU_PAD + y * W_Textheight, width * W_Textwidth, 
	   W_Textheight * height);
	break;
    }
}

void
W_ClearWindow(window)
    W_Window window;
{
    struct window * win = W_Void2Window(window);

#ifdef DEBUG
    printf("Clearing %d\n", window);
#endif
#ifdef BUFFERING
    if(win->isbuffered) {
      W_ClearBuffer(window);
    } else {
      XClearWindow(W_Display, win->window);
    }
#else 
    XClearWindow(W_Display, win->window);
#endif /*BUFFERING [BDyess]*/
}

