
#include "allincludes.h"

Cursor
make_cursor(bits, mask, width, height, xhot, yhot)
    char   *bits, *mask;
    unsigned int width, height, xhot, yhot;
{
    Pixmap  cursbits;
    Pixmap  cursmask;
    XColor  whiteCol, blackCol;
    Cursor  curs;

    whiteCol.pixel = colortable[W_White].pixelValue;
    XQueryColor(W_Display, W_Colormap, &whiteCol);
    blackCol.pixel = colortable[W_Black].pixelValue;
    XQueryColor(W_Display, W_Colormap, &blackCol);

    cursbits = XCreateBitmapFromData(W_Display, DefaultRootWindow(W_Display),
				     bits, width, height);
    cursmask = XCreateBitmapFromData(W_Display, DefaultRootWindow(W_Display),
				     mask, width, height);

    curs = XCreatePixmapCursor(W_Display, cursbits, cursmask,
			       &whiteCol, &blackCol, xhot, yhot);

    XFreePixmap(W_Display, cursbits);
    XFreePixmap(W_Display, cursmask);
    return curs;
}

void
W_DefineTCrossCursor(window)
    W_Window window;
{
    return;
}

void
W_DefineTextCursor(window)
    W_Window window;
{
    static Cursor new = 0;
    struct window *win = W_Void2Window(window);
    XColor  f, b;

    if (!new) {
	f.pixel = colortable[W_Yellow].pixelValue;
	b.pixel = colortable[W_Black].pixelValue;

	XQueryColor(W_Display, W_Colormap, &f);
	XQueryColor(W_Display, W_Colormap, &b);

	new = XCreateFontCursor(W_Display, XC_xterm);

	XRecolorCursor(W_Display, new, &f, &b);
    }
    XDefineCursor(W_Display, win->window, new);

    return;
}

void
W_RevertCursor(window)
    W_Window window;
{
    struct window *win = W_Void2Window(window);

    XDefineCursor(W_Display, win->window, win->cursor);

    return;
}

void
W_DefineCursor(window, width, height, bits, mask, xhot, yhot)
    W_Window window;
    int     width, height, xhot, yhot;
    char   *bits, *mask;
{
    return;
}

Cursor cursor;

void W_BlankCursor(window)
     W_Window window;
{
    struct window *win = W_Void2Window(window);
    
    XColor color;
    Pixmap cursorPixmap;
    
    color.pixel = WhitePixel(W_Display, DefaultScreen(W_Display));
    XQueryColor(W_Display, 
		DefaultColormap(W_Display, DefaultScreen(W_Display)), &color);
    
    /* Free old cursor */
    if (cursor) XFreeCursor(W_Display, cursor);

    cursorPixmap = XCreatePixmap(W_Display, win->window, 1, 1, 1);
    cursor = XCreatePixmapCursor(W_Display, 
				 cursorPixmap, cursorPixmap, &color, &color, 0, 0);
    if (cursorPixmap) XFreePixmap(W_Display, cursorPixmap);
    
    XDefineCursor(W_Display, win->window, cursor);
}
