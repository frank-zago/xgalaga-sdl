#include "allincludes.h"

void
W_TileWindow(window, image)
    W_Window window;
    W_Image  *image;
{
    Window  win;

    check_loaded(image);
#ifdef DEBUG
    printf("Tiling window %d\n", window);
#endif
    win = W_Void2Window(window)->window;

    if (image->xpm == 0) {
	Pixmap orig;
	orig = image->pixmap;
	image->pixmap = XCreatePixmap(W_Display, W_Root,
	      image->width, image->height, 
	      (unsigned)DefaultDepth(W_Display, W_Screen));
	XCopyPlane(W_Display, orig, image->pixmap,
	   colortable[W_White].contexts[0], 0, 0, image->width, image->height,
		   0, 0, 1);
	image->xpm = 1;
    }
    XSetWindowBackgroundPixmap(W_Display, win, image->pixmap);
    XClearWindow(W_Display, win);
}

void
W_UnTileWindow(window)
    W_Window window;
{
    Window  win;

#ifdef DEBUG
    printf("Untiling window %d\n", window);
#endif
    win = W_Void2Window(window)->window;

    XSetWindowBackground(W_Display, win, colortable[W_Black].pixelValue);
    XClearWindow(W_Display, win);
}

