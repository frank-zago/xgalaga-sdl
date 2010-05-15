#include "allincludes.h"

void
W_MapWindow(window)
    W_Window window;
{
    struct window *win;

#ifdef DEBUG
    printf("Mapping %d\n", window);
#endif
    win = W_Void2Window(window);
    if(win->type != WIN_BORDER && win->borderwin) {
      W_MapWindow(win->borderwin);
    }
    if (win->wm_window)
      W_MapWindow(win->wm_window);
    if (win->mapped)
	return;
    win->mapped = 1;
    XMapRaised(W_Display, win->window);
}

void
W_UnmapWindow(window)
    W_Window window;
{
    struct window *win;

#ifdef DEBUG
    printf("UnMapping %d\n", window);
#endif
    win = W_Void2Window(window);
    if (win->mapped == 0)
	return;
    win->mapped = 0;
    XUnmapWindow(W_Display, win->window);
    if(win->type != WIN_BORDER && win->borderwin) {
      W_UnmapWindow(win->borderwin);
    }
    if (win->wm_window)
      W_UnmapWindow(win->wm_window);
}

int
W_IsMapped(window)
    W_Window window;
{
    struct window *win;

    win = W_Void2Window(window);
    if (win == NULL)
	return (0);
    return (win->mapped);
}

static void
deleteWindow(window)
    struct window *window;
{
    struct windowlist **rm;
    struct windowlist *temp;

    rm = &hashtable[hash(window->window)];
    while (*rm != NULL && (*rm)->window != window) {
	rm = &((*rm)->next);
    }
    if (*rm == NULL) {
	printf("Attempt to delete non-existent window!\n");
	return;
    }
    temp = *rm;
    *rm = temp->next;
    free((char *) temp);
}

void
W_DestroyWindow(window)
    W_Window window;
{
    struct window *win;
    W_Window wm_window;
#ifdef DEBUG
    printf("Destroying %d\n", window);
#endif
    win = W_Void2Window(window);
    wm_window = win->wm_window;
    
    if(win->fs_window != None)
      W_ToggleFullscreen(window);
    
    deleteWindow(win);
    XDestroyWindow(W_Display, win->window);
    if(win->buffer) XFreePixmap(W_Display, win->buffer);
    if(win->type != WIN_BORDER && win->borderwin) 
      W_DestroyWindow(win->borderwin);
    free((char *) win);
    
    if (wm_window)
      W_DestroyWindow(wm_window);
}

void
W_ResizeWindow(window, neww, newh)	/* TSH 2/93 */
    W_Window window;
    int     neww, newh;
{
    struct window *win = W_Void2Window(window);

    XResizeWindow(W_Display, win->window, 
                  (unsigned int) neww, 
                  (unsigned int) newh);
    /* resize the border to match [BDyess] */
    XResizeWindow(W_Display, W_Void2Window(win->borderwin)->window, 
                  (unsigned int) neww + win->border * 2,
                  (unsigned int) newh + win->border * 2);
}

void
W_Deiconify(window)
    W_Window window;
{
    struct window *win;
    win = W_Void2Window(window);
    /* according to ICCCM 4.1.4, this is how you deiconify a window. */
    XMapWindow(W_Display, win->window);
}

