#include "allincludes.h"

void
W_ChangeBorder(window, color)
    W_Window window;
    int     color;
{
#ifdef DEBUG
    printf("Changing border of %d\n", window);
#endif

#if 0
    /* fix inexplicable color bug */
    if (DisplayCells(W_Display, W_Screen) <= 2)
	XSetWindowBorderPixmap(W_Display, W_Void2Window(window)->window,
			       colortable[color].pixmap);
    else
	XSetWindowBorder(W_Display, W_Void2Window(window)->window,
			 colortable[color].pixelValue);
#endif /*0*/

    struct window *border = W_Void2Window(W_Void2Window(window)->borderwin);

    border->border_color = color;
    redrawBorder(border);
}

/* this function will redraw a border window [BDyess] */
 void
redrawBorder(win)
    struct window *win;
{
    XPoint points[3];
    XGCValues val;

    if(win->border == 0) return;
    val.line_width = win->border*2 + 1;
    if(win->border_color == NONE) {
      /* GC is set up so that lines are drawn with thickness BORDER, so only one
	 line is needed for each side.  The background is set to DARK_GREY,
	 which is the color of the left and top sides, so only two other
	 lines need to be drawn. [BDyess] */
      XChangeGC(W_Display, borderGC, GCLineWidth, &val);
      points[0].x = win->border/2;
      points[0].y = win->height;
      points[1].x = win->width;
      points[1].y = win->height;
      points[2].x = win->width;
      points[2].y = win->border/2;
      XDrawLines(W_Display, win->drawable, borderGC, points, 3,
                 CoordModeOrigin);
    } else {	/* draw a color border */
      val.foreground = colortable[win->border_color].pixelValue;
      XChangeGC(W_Display, borderGC, GCLineWidth | GCForeground, &val);
      XDrawRectangle(W_Display, win->drawable, borderGC, win->border - 1,
		     win->border - 1, 
		     win->width - 2 * win->border + 2,
		     win->height - 2 * win->border + 2);
      XSetForeground(W_Display, borderGC, colortable[LIGHT_GREY].pixelValue);
    }
}

/* this function will redraw the main (background) window, which contains a 
   reverse-border.  [BDyess] */
#if NOTUSED
 void
redrawReversedBorder(win)
    struct window *win;
{
    XPoint points[3];
    XGCValues val;

    /* GC is set up so that lines are drawn with thickness BORDER, so only one
       line is needed for each side.  The background is set to DARK_GREY,
       which is the color of the left and top sides, so only two other
       lines need to be drawn. [BDyess] */
    val.line_width = BORDER*2 + 1;
    XChangeGC(W_Display, borderGC, GCLineWidth, &val);
    points[0].x = 0;
    points[0].y = win->height - BORDER/2;
    points[1].x = 0;
    points[1].y = 0;
    points[2].x = win->width - BORDER/2;
    points[2].y = 0; 
    XDrawLines(W_Display, win->drawable, borderGC, points, 3, CoordModeOrigin);
}
#endif

