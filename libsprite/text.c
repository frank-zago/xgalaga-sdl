#include "allincludes.h"

/* W_ShadowText: draw text with a black shadow underneath.  Just calls
   W_WriteText twice 
 */
void W_ShadowText(window, x, y, color, str, len, font)
    W_Window window;
    int     x, y, len;
    W_Color color;
    W_Font  font;
    char   *str;
{
    W_FillArea(window, x, y, len * W_Textwidth, W_Textheight, DARK_GREY);
    W_MaskText(window, x-1, y+1, W_Black, str, len, font);
    W_MaskText(window, x, y, color, str, len, font);
}

void
W_WriteText(window, x, y, color, str, len, font)
    W_Window window;
    int     x, y, len;
    W_Color color;
    W_Font  font;
    char   *str;
{
    struct window *win;
    int     addr;

    if (!font)
	font = W_RegularFont;
#ifdef DEBUG
    printf("Text for %d @ (%d, %d) in %d: [%s]\n", window, x, y, color, str);
#endif
    win = W_Void2Window(window);
    switch (win->type) {
    case WIN_GRAPH:
	addr = fonts[fontNum(font)].baseline;
	XDrawImageString(W_Display, win->drawable,
	  colortable[color].contexts[fontNum(font)], x, y + addr, str, len);
	break;
    case WIN_SCROLL:
	if (y<0) {
	  XCopyArea(W_Display, win->drawable, win->drawable,
		    colortable[W_White].contexts[0], WIN_EDGE, MENU_PAD,
		    win->width * W_Textwidth, (win->height - 1) * W_Textheight,
		    WIN_EDGE, MENU_PAD+W_Textheight);
	  XClearArea(W_Display, win->window,
		     WIN_EDGE, MENU_PAD,
		     W_Textwidth * win->width,(unsigned) W_Textheight, False);
	  XDrawImageString(W_Display, win->drawable,
			   colortable[color].contexts[1],
			   WIN_EDGE, MENU_PAD + fonts[1].baseline,
			   str, len);
	} else {
	  XCopyArea(W_Display, win->drawable, win->drawable,
		    colortable[W_White].contexts[0], WIN_EDGE, MENU_PAD + W_Textheight,
		    win->width * W_Textwidth, (win->height - 1) * W_Textheight,
		    WIN_EDGE, MENU_PAD);
	  XClearArea(W_Display, win->window,
		     WIN_EDGE,(int)(MENU_PAD+W_Textheight*(win->height-1)),
		     W_Textwidth * win->width, (unsigned)W_Textheight, False);
	  XDrawImageString(W_Display, win->drawable,
			   colortable[color].contexts[1],
			   WIN_EDGE, 
			   (int)(MENU_PAD + W_Textheight * (win->height-1)
			            + fonts[1].baseline),
			   str, len);
	}
	AddToScrolling(win, color, str, len);
	break;
    case WIN_MENU:
	changeMenuItem(win, y, color, str, len, font);
	break;
    default:
	addr = fonts[fontNum(font)].baseline;
	XDrawImageString(W_Display, win->drawable,
			 colortable[color].contexts[fontNum(font)],
	     x * W_Textwidth + WIN_EDGE, MENU_PAD + y * W_Textheight + addr,
			 str, len);
	break;
    }
}

void
W_MaskText(window, x, y, color, str, len, font)
    W_Window window;
    int     x, y, len;
    W_Color color;
    W_Font  font;
    char   *str;
{
    struct window *win;
    int     addr;

    addr = fonts[fontNum(font)].baseline;
#ifdef DEBUG
    printf("TextMask for %d @ (%d, %d) in %d: [%s]\n", window, x, y, color, str);
#endif
    win = W_Void2Window(window);
    XDrawString(W_Display, win->drawable,
	  colortable[color].contexts[fontNum(font)], x, y + addr, str, len);
}

/* same as W_MaskText above, except draws directly to the destination window
   and not its drawable.  This bypasses buffering, if any. [BDyess] */
void
W_DirectMaskText(window, x, y, color, str, len, font)
    W_Window window;
    int     x, y, len;
    W_Color color;
    W_Font  font;
    char   *str;
{
    struct window *win;
    int     addr;

    addr = fonts[fontNum(font)].baseline;
#ifdef DEBUG
    printf("TextMask for %d @ (%d, %d) in %d: [%s]\n", window, x, y, color, str);
#endif
    win = W_Void2Window(window);
    XDrawString(W_Display, win->window,
	  colortable[color].contexts[fontNum(font)], x, y + addr, str, len);
}

/* find the width of a font */
#if 1
int
W_StringWidth(string, font)
    char    string[];
    W_Font  font;
{
    int     x, y;

    y = strlen(string);
    x = XTextWidth(fonts[fontNum(font)].fontstruct, string, y);
    return (x);			/* just a guess ?? old never returned! WHS
				   4/6/93 */
}
#endif /*0*/

void
W_ResizeText(window, neww, newh)/* TSH 2/93 */
    W_Window window;
    int     neww, newh;
{
    W_ResizeWindow(window, neww * W_Textwidth + WIN_EDGE * 2,
		   newh * W_Textheight + MENU_PAD * 2);
}

