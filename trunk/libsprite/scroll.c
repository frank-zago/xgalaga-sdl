#include "allincludes.h"

/* Add a string to the string list of the scrolling window.
 */
 void
AddToScrolling(win, color, str, len)
    struct window *win;
    W_Color color;
    char   *str;
    int     len;
{
    char   *newstring;
    char  **p = & (win->data);
    struct stringList **strings = (struct stringList **)p ;
    int count = 0;

    while ((*strings) != NULL) {
	count++;
	strings = &((*strings)->next);
    }
    (*strings) = (struct stringList *) malloc(sizeof(struct stringList));
    (*strings)->next = NULL;
    (*strings)->color = color;
    newstring = (char *) malloc((unsigned)(len + 1));
    strncpy(newstring, str, (unsigned)len);
    newstring[len] = 0;
    (*strings)->string = newstring;
    if (count >= 100) {		/* Arbitrary large size. */
	struct stringList *temp;

	temp = (struct stringList *) win->data;
	free(temp->string);
	temp = temp->next;
	free((char *) win->data);
	win->data = (char *) temp;
    }
}

 void
redrawScrolling(win)
    struct window *win;
{
    int     count;
    struct stringList *list;
    int     y;

#ifdef BUFFERING
    if(win->isbuffered) {
      W_DisplayBuffer(W_Window2Void(win));
      return;
    }
#endif /*BUFFERING [BDyess]*/

    XClearWindow(W_Display, win->window);
    count = 0;
    list = (struct stringList *) win->data;
    while (list != NULL) {
	list = list->next;
	count++;
    }
    list = (struct stringList *) win->data;
    while (count > win->height) {
	list = list->next;
	count--;
    }
    y = (win->height - count) * W_Textheight + fonts[1].baseline;
    if (count == 0)
	return;
    while (list != NULL) {
	XDrawImageString(W_Display, win->drawable,
			 colortable[list->color].contexts[1],
		WIN_EDGE, MENU_PAD + y, list->string,(int)strlen(list->string));
	list = list->next;
	y = y + W_Textheight;
    }
}

void
W_SetSensitive(window, v)
    W_Window window;
    int     v;
{
    struct window *win = W_Void2Window(window);

    win->insensitive = !v;

    if (win->type == WIN_SCROLL)
	redrawScrolling(win);
}

 void
resizeScrolling(win, width, height)
    struct window *win;
    int     width, height;
{
    win->height = (height - MENU_PAD * 2) / W_Textheight;
    win->width = (width - WIN_EDGE * 2) / W_Textwidth;
    XResizeWindow(W_Display, win->window, win->width * W_Textwidth + WIN_EDGE * 2,
		  win->height * W_Textheight + MENU_PAD * 2);
}

