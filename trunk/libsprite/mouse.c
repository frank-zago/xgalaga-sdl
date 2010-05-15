#include "allincludes.h"

static void
findMouse(x, y)
    int    *x, *y;
{
    Window  theRoot, theChild;
    int     wX, wY, rootX, rootY, ErrorVal;
    unsigned int wButtons;

    ErrorVal = XQueryPointer(W_Display, W_Root, &theRoot, &theChild, &rootX, &rootY, &wX, &wY, &wButtons);
    if (ErrorVal == True) {
	*x = wX;
	*y = wY;
    } else {
	*x = 0;
	*y = 0;
    }
}

void
W_WarpPointer(window, x, y)
    W_Window window;
    int     x, y;
{
    static int warped_from_x = 0, warped_from_y = 0;

    if (window == NULL) {
	if (W_in_message) {
	    XWarpPointer(W_Display, None, W_Root, 0, 0, 0, 0, warped_from_x, warped_from_y);
	    W_in_message = 0;
	}
    } else {
	findMouse(&warped_from_x, &warped_from_y);
	XWarpPointer(W_Display, None, W_Void2Window(window)->window, 0, 0, 0, 0, 0, 0);
	W_in_message = 1;
    }
}

int
findMouseInWin(x, y, window)
    int    *x, *y;
    W_Window window;
{
    Window  theRoot, theChild;
    int     wX, wY, rootX, rootY, ErrorVal;
    unsigned int wButtons;
    struct window *win = W_Void2Window(window);
    Window  thisWin = win->window;

    ErrorVal = XQueryPointer(W_Display, thisWin, &theRoot, &theChild,
			   &rootX, &rootY, &wX, &wY, &wButtons);
    if (ErrorVal == True) {
	/*
	   if it's in the window we specified then the values returned should
	   be within the with and height of the window
	*/
	if (wX <= win->width && wY <= win->height) {
	    *x = wX;
	    *y = wY;
	    return 1;
	}
    }
    *x = 0;
    *y = 0;
    return 0;
}

void
W_GetMouse(window, x, y, but)
     W_Window window;
     int *x, *y, *but;
{
    struct window *win = W_Void2Window(window);
    Window QueryRoot, QueryChild;
    int AbsoluteX, AbsoluteY;
    unsigned int	        ModKeyMask;

    XQueryPointer( W_Display, win->window, &QueryRoot, &QueryChild, &AbsoluteX, &AbsoluteY, x, y, &ModKeyMask );

    if(ModKeyMask & Button1Mask)
	*but = W_LBUTTON;
    else
	*but = 0;
}

void
W_GrabPointer(window)
     W_Window window;
{
    struct window *win = W_Void2Window(window);

    XGrabPointer(W_Display, win->window, True, 0, GrabModeAsync, GrabModeAsync, win->window, None, CurrentTime);
}

void
W_UngrabPointer()
{
    XUngrabPointer(W_Display, CurrentTime);
}
