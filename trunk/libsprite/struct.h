#ifndef _struct_h_
#define _struct_h_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmu/CurUtil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include <assert.h>
#include <string.h>

#ifdef XPM
#ifdef HACKED_XPMLIB
#include "xpmlib-3.4b/xpm.h"
#else 
#include <X11/xpm.h>
#endif /*HACKED_XPMLIB*/
#endif /*XPM [BDyess]*/

#include "defs.h"

struct fontInfo {
    XFontStruct *fontstruct;
    int     baseline;
};

struct colors {
    char   *name;
    GC      contexts[FONTS + NONFONTS];
    Pixmap  pixmap;
    unsigned long pixelValue;
};

struct window {
    Window  window;
    W_Window borderwin;
    int     border;
    W_Color border_color;
#ifdef BUFFERING
    Drawable drawable;
    Pixmap  buffer;
    int     isbuffered;
#endif /*BUFFERING [BDyess]*/
    int     type;
    char   *data;
    int     mapped;
    unsigned int width, height;
    char   *name;
    Cursor  cursor;
    int     insensitive;
#if 0
    W_Callback handle_keydown;
    W_Callback handle_keyup;
    W_Callback handle_button;
    W_Callback handle_expose;
#endif				/* 0 */
    W_Window wm_window;
    Window fs_window;
};

struct stringList {
    char   *string;
    W_Color color;
    struct stringList *next;
};

struct menuItem {
    char   *string;
    W_Color color;
    W_Font  font;
};

struct windowlist {
    struct window *window;
    struct windowlist *next;
};

#endif /* _struct_h_ */
