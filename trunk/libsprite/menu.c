#include "allincludes.h"

W_Window
W_MakeMenu(name, x, y, width, height, parent, border)
    char   *name;
    int     x, y, width, height;
    W_Window parent;
    int     border;
{
    return (W_Window)w_MakeWindow(name, x, y, width, height, parent,
			"left_ptr", border, W_White, WIN_MENU);
}

static void
redrawMenuItem(win, n)
    struct window *win;
    int     n;
{
    struct menuItem *items;
    int     addr;

    items = (struct menuItem *) win->data;

    XFillRectangle(W_Display, win->drawable,
		   colortable[W_Black].contexts[0],
	  WIN_EDGE, n * (W_Textheight + MENU_PAD * 2 + MENU_BAR) + MENU_PAD,
		   win->width * W_Textwidth, (unsigned)W_Textheight);
    if (items[n].string) {
	addr = fonts[fontNum(items[n].font)].baseline;
	XDrawImageString(W_Display, win->drawable,
		colortable[items[n].color].contexts[fontNum(items[n].font)],
			 WIN_EDGE,
	     n * (W_Textheight + MENU_PAD * 2 + MENU_BAR) + MENU_PAD + addr,
			 items[n].string, (int)strlen(items[n].string));
    }
}

 void
redrawMenu(win)
    struct window *win;
{
    int     count;

    for (count = 1; count < win->height; count++) {
	XFillRectangle(W_Display, win->drawable,
		       colortable[W_Grey].contexts[0],
	  0, count * (W_Textheight + MENU_PAD * 2) + (count - 1) * MENU_BAR,
		       win->width * W_Textwidth + WIN_EDGE * 2, MENU_BAR);
    }
    for (count = 0; count < win->height; count++) {
	redrawMenuItem(win, count);
    }
}

 void
changeMenuItem(win, n, color, str, len, font)
    struct window *win;
    int     n;
    W_Color color;
    char   *str;
    int     len;
    W_Font  font;
{
    struct menuItem *items;
    char   *news;

    items = (struct menuItem *) win->data;
    if (items[n].string) {
	free(items[n].string);
    }
    news = malloc((unsigned)(len + 1));
    strncpy(news, str, (unsigned)len);
    news[len] = 0;
    items[n].string = news;
    items[n].color = color;
    items[n].font = font;
    redrawMenuItem(win, n);
    XFlush(W_Display);
}

void
W_ResizeMenu(window, neww, newh)/* TSH 2/93 */
    W_Window window;
    int     neww, newh;
{
    W_ResizeWindow(window, neww * W_Textwidth + WIN_EDGE * 2,
	      newh * (W_Textheight + MENU_PAD * 2) + (newh - 1) * MENU_BAR);
}

