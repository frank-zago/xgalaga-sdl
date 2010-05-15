#include "allincludes.h"

#ifdef BUFFERING
/* clears the buffer [BDyess] */
void
W_ClearBuffer(window)
    W_Window window;
{
  struct window * win = W_Void2Window(window);

  if(!win->isbuffered) return;
  XFillRectangle(W_Display, win->drawable, colortable[backColor].contexts[0],
		 0, 0, win->width, win->height);
}

int
W_IsBuffered(window)
    W_Window window;
{
  return W_Void2Window(window)->isbuffered;
}

/* turns on buffering, reduces flicker [BDyess] */
/* on is a flag: 1 turns on buffering, 0 turns it off */
void
W_Buffer(window, on)
    W_Window window;
    int on;
{
  struct window * win = W_Void2Window(window);

  if(on) {	/* turn buffering on [BDyess] */
    win->isbuffered = 1;
    if(win->buffer == 0) {  /* create a new pixmap for the buffer [BDyess] */
      win->buffer = XCreatePixmap(W_Display, W_Root, win->width,
				win->height, 
				(unsigned)DefaultDepth(W_Display, W_Screen));
    }
    win->drawable = win->buffer;
    /* clear the buffer to start with (can contain garbage) [BDyess] */
    W_ClearBuffer(window);
  } else { /* turn it off [BDyess] */
    win->drawable = win->window;
    win->isbuffered = 0;
  }
}

/* draws the buffer onto the screen [BDyess] */
void
W_DisplayBuffer(window)
    W_Window window;
{
  struct window * win = W_Void2Window(window);

  if(!win->isbuffered) return;
  XCopyArea(W_Display, win->buffer, win->window, 
            colortable[W_Black].contexts[0], 0, 0, win->width, 
	    win->height, 0, 0);
}
#endif /*BUFFERING [BDyess]*/
