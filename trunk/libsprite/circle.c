#include "allincludes.h"

void
W_DrawCircle(window, centerx, centery, diameter, color)
    W_Window window;
    unsigned int diameter;
    int centerx, centery;
    W_Color color;
{
    XDrawArc(W_Display, W_Void2Window(window)->drawable, 
    	     colortable[color].contexts[0], 
	     (int)(centerx - diameter / 2), 
	     (int)(centery - diameter / 2), 
	     diameter - 1, diameter - 1, 0, 360 * 64);
}
   
/* needed by newstats.c [BDyess] */
void
W_WriteArc(filled, window, x, y, width, height, angle1, angle2, color)
    int      filled;
    W_Window window;
    int      x, y;
    int      width, height;
    int      angle1, angle2;
    W_Color  color;
{
    static GC pen = 0;
    struct window *win;
    unsigned long valuemask = 0;    /* Ignore XGCvalues and use defaults */
    XGCValues values;

    win = W_Void2Window(window);

    if(pen == 0) pen = XCreateGC(W_Display, W_Root, valuemask, &values);
    XSetForeground(W_Display, pen, colortable[color].pixelValue);
    if (filled) {
       XFillArc(W_Display, win->window, pen, x - (int)(width/2),
             y - (int)(height/2), width, height, angle1 * 64, angle2 * 64);
    } else {
       XDrawArc(W_Display, win->window, pen, x - (int)(width/2),
             y - (int)(height/2), width, height, angle1 * 64, angle2 * 64);
    }

    return;
}

