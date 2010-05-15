#include "allincludes.h"

#define tiny_width 3
#define tiny_height 3
#define tiny_x_hot 1
#define tiny_y_hot 1
static char tiny_bits[] = {
  0x05, 0x02, 0x05, };
static char tinymask_bits[] = {
  0x05, 0x02, 0x05, };

#define cross_width 16
#define cross_height 16
#define cross_x_hot 7
#define cross_y_hot 7
static char cross_bits[] = {
    0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xc0, 0x01, 0x80, 0x00,
    0x10, 0x04, 0x3f, 0x7e, 0x10, 0x04, 0x80, 0x00, 0xc0, 0x01, 0x80, 0x00,
0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00};
static char crossmask_bits[] = {
    0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xe0, 0x03, 0xd0, 0x05,
    0xbf, 0x7e, 0x7f, 0x7f, 0xbf, 0x7e, 0xd0, 0x05, 0xe0, 0x03, 0xc0, 0x01,
0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0x00, 0x00};

static void
checkGeometry(name, x, y, width, height)
    char   *name;
    int    *x, *y, *width, *height;
/* fixed so that it handles negative positions 12/21/93          */
/* note that this is NOT standard X syntax, but it was requested */
/* and it's how BRM-Hadley works.                       [BDyess] */
{
    char   *adefault;
    char    buf[100];
    char   *s;

#ifdef RJC
    *x = *y = INVALID_POSITION;
#endif				/* RJC */

    sprintf(buf, "%s.geometry", name);
    adefault = NULL; /*stringDefault(buf,NULL);*/
    if (adefault == NULL)
	return;
    /* geometry should be of the form 502x885+1+1, 502x885, or +1+1 */
    s = adefault;
    if (*s != '+' && *s != '-') {
	while (*s != 'x' && *s != 0)
	    s++;
	*width = atoi(adefault);
	if (*s == 0)
	    return;
	s++;
	adefault = s;
	while (*s != '+' && *s != '-' && *s != 0)
	    s++;
	*height = atoi(adefault);
	if (*s == 0)
	    return;
    }
    adefault = s;
    s++;
    if (*s == '-')
	s++;			/* for the case where they have wxh+-x+y */
    while (*s != '+' && *s != '-' && *s != 0)
	s++;
    *x = atoi(adefault + 1);
    if (*adefault == '-')
	*x = -*x;
    if (*s == 0)
	return;
    *y = atoi(s + 1);
    if (*s == '-')
	*y = -*y;
    /* printf("width: %d, height: %d, x: %d, y: %d\n",*width, *height,*x,*y); */
    return;
}

static  void
checkParent(name, parent)
    char   *name;
    W_Window *parent;
{
    char   *adefault;
    char    buf[100];
    int     i;
    struct windowlist *windows;

    sprintf(buf, "%s.parent", name);
    adefault = NULL; /*stringDefault(buf,NULL);*/
    if (adefault == NULL)
	return;
    /* parent must be name of other window or "root" */
    if (strcasecmp(adefault, "root") == 0) {
	*parent = W_Window2Void(&myroot);
	return;
    }
    for (i = 0; i < HASHSIZE; i++) {
	windows = hashtable[i];
	while (windows != NULL) {
	    if (strcasecmp(adefault, windows->window->name) == 0) {
		*parent = W_Window2Void(windows->window);
		return;
	    }
	    windows = windows->next;
	}
    }
}

static void
checkCursor(name, cursname, cursor)
    char   *name;
    char   *cursname;
    Cursor *cursor;
{
    char    buf[100];
    char   *adefault;

    *cursor = 0;

    sprintf(buf, "%s.cursor", name);
    adefault = cursname; /*stringDefault(buf,cursname);*/

    if (adefault == NULL)
	return;

#ifdef RFCURSORS
    cnum = XmuCursorNameToIndex(adefault);
    if (cnum != -1) {
	XColor  f, b;
	*cursor = XCreateFontCursor(W_Display, cnum);
	if (cnum == XC_xterm) {

	    f.pixel = colortable[W_Yellow].pixelValue;
	    b.pixel = colortable[W_Black].pixelValue;

	    XQueryColor(W_Display, W_Colormap, &f);
	    XQueryColor(W_Display, W_Colormap, &b);

	    XRecolorCursor(W_Display, *cursor, &f, &b);
	} else if (cnum == XC_pirate) {
	    f.pixel = colortable[W_Red].pixelValue;
	    b.pixel = colortable[W_Black].pixelValue;

	    XQueryColor(W_Display, W_Colormap, &f);
	    XQueryColor(W_Display, W_Colormap, &b);

	    XRecolorCursor(W_Display, *cursor, &f, &b);
	}
    } else
#endif
    if (0 == strcmp("bomb here", adefault)) {
	static Cursor bomb_here = 0;
	if (bomb_here == 0) {
	    bomb_here = make_cursor(cross_bits, crossmask_bits,
				    cross_width, cross_height,
				    cross_x_hot, cross_y_hot);
	}
	*cursor = bomb_here;
    } else if (0 == strcmp("tiny", adefault)) {
	static Cursor tiny = 0;
	if(tiny == 0) {
	    tiny = make_cursor(tiny_bits, tinymask_bits,
			       tiny_width, tiny_height,
			       tiny_x_hot, tiny_y_hot);
	}
	*cursor = tiny;
    }
}

static int
checkMapped(name)
    char   *name;
{
    char    buf[100];

    sprintf(buf, "%s.mapped", name);
    return 0;
}

#ifdef BUFFERING
static int
checkBuffered(name)
    char *name;
{
    char    buf[100];

    sprintf(buf, "%s.buffered", name);
#if 0
    /* defaults to on when in xpm mode for local window [BDyess] */
    /* don't turn it on unless asked when in OR mode [BDyess] */
    if(xpm && !useOR && strcmp(name,"local") == 0) {
      printf("Double Buffering automatically enabled.  Use -O if your machine can't handle it.\n");
      return (booleanDefault(buf, 1));
    } else {
      return (booleanDefault(buf, 0));
    }
#else
    return useBuffered;
#endif /*0*/
}
#endif /*BUFFERING [BDyess]*/

static void
addToHash(win)
    struct window *win;
{
    struct windowlist **new;

#ifdef DEBUG
    printf("Adding to %d\n", hash(win->window));
#endif
    new = &hashtable[hash(win->window)];
    while (*new != NULL) {
	new = &((*new)->next);
    }
    *new = (struct windowlist *) malloc(sizeof(struct windowlist));
    (*new)->next = NULL;
    (*new)->window = win;
}

struct window *
newWindow(window, type)
    Window  window;
    int     type;
{
    struct window *neww;

    neww = (struct window *) malloc(sizeof(struct window));
    neww->window = window;
    neww->drawable = window;
#ifdef BUFFERING
    neww->isbuffered = 0;
    neww->buffer = 0;
#endif /*BUFFERING [BDyess]*/
    neww->type = type;
    neww->mapped = 0;
    neww->insensitive = 0;
/*#ifdef XF86VIDMODE*/
    neww->wm_window = NULL;
    neww->fs_window = None;
/*#endif*/
    addToHash(neww);
    return (neww);
}

void
W_RenameWindow(window, str)
    W_Window window;
    char   *str;
{
    XStoreName(W_Display, ((struct window *) window)->window, str);
}

  W_Window
w_MakeWindow(name, x, y, width, height, parent,
	     cursname, border, color, wsort)
    char   *name;
    int     x, y, width, height;
    W_Window parent;
    char   *cursname;
    int     border;
    W_Color color;		/* unused */
    int     wsort;		/* WIN_? */
{
    int     gx, gy;
    struct window *neww;
    Window  wparent;
    W_Window borderwin = 0;
    Cursor  cursor;
    XSetWindowAttributes attrs;
    unsigned int pwidth, pheight;	/* pixel width and height */

    if(wsort != WIN_BORDER) {
      checkGeometry(name, &gx, &gy, &width, &height);
      if (gx != INVALID_POSITION)
	  x = gx;
      if (gy != INVALID_POSITION)
	  y = gy;

      checkParent(name, &parent);

      if (wsort == WIN_TEXT || wsort == WIN_SCROLL || wsort == WIN_MENU) {
	  pwidth = width * W_Textwidth + WIN_EDGE * 2;
	  if (wsort == WIN_MENU)
	      pheight = height * (W_Textheight + MENU_PAD * 2 + MENU_BAR) - MENU_BAR;
	  else
	      pheight = height * W_Textheight + MENU_PAD * 2;
      } else {
	  pwidth = width;
	  pheight = height;
      }

      /* if this isn't a border, create one with another call to this 
	 function [BDyess] */
      if (border) {
#if 0
	char *newname = (char*) malloc (strlen(name) + 8);

	strcpy(newname,name);
	strcat(newname,"_border");
	free(newname);
#endif /*0*/
	parent = w_MakeWindow(name, x, y, pwidth+border*2, pheight+border*2, 
			      parent, cursname, border, color, WIN_BORDER);
	borderwin = parent;
	/* update the parameters to reflect the size of the surrounding burder
	   [BDyess] */
	x = border;
	y = border;
      }
      attrs.background_pixel = colortable[W_Black].pixelValue;
    } else { /* it is a border, set the background [BDyess] */
      pwidth = width;
      pheight = height;
      attrs.background_pixel = colortable[DARK_GREY].pixelValue;
    }

    wparent = W_Void2Window(parent)->window;

    checkCursor(name, cursname, &cursor);
    attrs.cursor = cursor;

    switch (wsort) {
    case WIN_TEXT:
    case WIN_MENU:
	attrs.event_mask = KeyPressMask | ButtonPressMask | ExposureMask | ButtonReleaseMask;
	attrs.do_not_propagate_mask = ExposureMask | KeyPressMask | ButtonPressMask;
	break;
    case WIN_GRAPH:
	attrs.event_mask = KeyPressMask | ButtonPressMask | ExposureMask | LeaveWindowMask | ButtonReleaseMask | ButtonMotionMask;
	attrs.do_not_propagate_mask = ExposureMask;
	break;
    case WIN_SCROLL:
	attrs.event_mask = ResizeRedirectMask | ExposureMask | KeyPressMask | ButtonReleaseMask | ButtonPressMask;
	attrs.do_not_propagate_mask = ResizeRedirectMask | ExposureMask;
	break;
    case WIN_BORDER:
        attrs.event_mask = ExposureMask;
	attrs.do_not_propagate_mask = ExposureMask;
	break;
    default:
	fprintf(stderr, "x11window.c: w_MakeWindow: unknown wsort %d\n", wsort);
    }

#ifdef AUTOKEY
    if (attrs.event_mask & KeyPressMask)
	attrs.event_mask |= KeyReleaseMask;
#endif				/* AUTOKEY */

    if (strcmp(name, "xgalaga_icon") == 0)	/* icon should not select for
						   input */
	attrs.event_mask = ExposureMask;
    if (strcmp(name, "wait_icon") == 0)	/* same here [BDyess] */
	attrs.event_mask = ExposureMask;

    if (strcmp(name, "info") == 0)	/* make info window passthru [BDyess] */
	attrs.event_mask = ExposureMask;

    neww = newWindow
	(XCreateWindow(W_Display, wparent, x, y, pwidth, pheight,
		      (unsigned) 0,
		       CopyFromParent, InputOutput, CopyFromParent,
		       (unsigned)(CWBackPixel | CWEventMask |
		       (cursor ? CWCursor : 0)),
		       &attrs),
	 wsort);

    neww->cursor = cursor;
    /* keep track of each windows border so they can be mapped and unmapped
       together. [BDyess] */
    neww->borderwin = borderwin;
    neww->border = border;
    neww->border_color = NONE;

    {
	char   *s;

	s = name;

	XStoreName(W_Display, neww->window, s);
    }

    wm_size_hint.width = wm_size_hint.min_width =
	wm_size_hint.max_width = wm_size_hint.base_width = pwidth;
    wm_size_hint.min_height = wm_size_hint.height =
	wm_size_hint.max_height = wm_size_hint.base_height = pheight;
    wm_size_hint.flags = USSize | PMinSize | PMaxSize | PBaseSize;
    if (gx > INVALID_POSITION || gy > INVALID_POSITION) {
	wm_size_hint.flags |= USPosition;
	wm_size_hint.x = x;
	wm_size_hint.y = y;
    }
    XSetWMNormalHints(W_Display, neww->window, &wm_size_hint);

    class_hint.res_name = name;
    class_hint.res_class = "XGalaga";
    XSetClassHint(W_Display, neww->window, &class_hint);

    XSetWMHints(W_Display, neww->window, &wm_hint);

    if (((wparent == W_Root &&
	  baseWin != NULL &&
	  strcmp(name, "wait") != 0)
	 || wsort == WIN_MENU) &&
	strcmp(name, "MetaServer List") != 0 &&
	strcmp(name, "Motd") != 0) {
	XSetTransientForHint(W_Display, neww->window,
			     W_Void2Window(baseWin)->window);
    }
    neww->name = strdup(name);
    neww->width = width;
    neww->height = height;
    if (wsort == WIN_MENU) {
	int     i;
	struct menuItem *items;
	items = (struct menuItem *) malloc(height * sizeof(struct menuItem));
	for (i = 0; i < height; i++) {
	    items[i].string = NULL;
	    items[i].color = W_White;
	    items[i].font = W_RegularFont;
	}
	neww->data = (char *) items;
    } else {
	neww->data = 0;
    }

    if (wparent != W_Root && wsort != WIN_BORDER)
	if (checkMapped(name))
	    W_MapWindow(W_Window2Void(neww));

#ifdef BUFFERING
    /* turn on buffering if name.buffered: on [BDyess] */
    if(wsort != WIN_BORDER) {
      if(checkBuffered(name)) {
	W_Buffer(W_Window2Void(neww), 1);
      }
    }
#endif /*BUFFERING [BDyess]*/

#ifdef DEBUG
    printf("New graphics window %d, child of %d\n", neww, parent);
#endif

#ifdef FOURPLANEFIX
    XSetWindowColormap(W_Display, neww->window, W_Colormap);
#endif

    return (W_Window2Void(neww));
}

W_Window
W_MakeWindow(name, x, y, width, height, parent, cursname, border, color)
    char   *name;
    int     x, y, width, height;
    W_Window parent;
    char   *cursname;
    int     border;
    W_Color color;
{
    /* When creating a toplevel window create a real toplevel
       window and a child window of the same dimensions. Then when switching
       fullscreen we can create a fullscreen window and reparent the child
       window to the fullscreen window and back again when switching to
       windowed mode again. */
    if(parent == NULL)
    {
      W_Window wm_window = w_MakeWindow(name, x, y, width, height, 0,
                              cursname, border, color, WIN_GRAPH);
      struct window *child_window = (struct window *)w_MakeWindow(
                                      name, x, y, width, height,
                                      wm_window, cursname, border, color,
                                      WIN_GRAPH);
      child_window->wm_window = wm_window;
      return (W_Window)child_window;
    }
    return w_MakeWindow(name, x, y, width, height, parent,
			cursname, border, color, WIN_GRAPH);
}

W_Window
W_MakeTextWindow(name, x, y, width, height, parent, cursname, border)
    char   *name;
    int     x, y, width, height;
    W_Window parent;
    char   *cursname;
    int     border;
{
    return w_MakeWindow(name, x, y, width, height,
			parent, cursname, border, W_White, WIN_TEXT);
}



W_Window
W_MakeScrollingWindow(name, x, y, width, height, parent, cursname, border)
    char   *name;
    int     x, y, width, height;
    W_Window parent;
    char   *cursname;
    int     border;
{
    return w_MakeWindow(name, x, y, width, height, parent, cursname,
			border, W_White, WIN_SCROLL);
}

void
W_SetIconWindow(win, icon)
    W_Window win;
    W_Window icon;
{
    XWMHints hints;

    XSetIconName(W_Display, W_Void2Window(icon)->window, W_Void2Window(win)->name);

    hints.flags = IconWindowHint;
    hints.icon_window = W_Void2Window(icon)->window;
    XSetWMHints(W_Display, W_Void2Window(win)->window, &hints);
}

#ifdef XF86VIDMODE
/* qsort comparison function for sorting the modes */
static int cmpmodes(const void *va, const void *vb)
{
    const XF86VidModeModeInfo *a = *(const XF86VidModeModeInfo **)va;
    const XF86VidModeModeInfo *b = *(const XF86VidModeModeInfo **)vb;
    if ( a->hdisplay == b->hdisplay )
        return b->vdisplay - a->vdisplay;  
    else
        return b->hdisplay - a->hdisplay;
}
#endif

void W_ToggleFullscreen(_win)
    W_Window _win;
{
#ifdef XF86VIDMODE      
  int i;
  XF86VidModeModeLine mode;
  XF86VidModeModeInfo **modes;
  int mode_count;
#endif
  struct window *win = (struct window *)_win;
  static int mouse_grabbed = 0;
  static int keyboard_grabbed = 0;

  /* Check we get a nested toplevel WIN_GRAPH window passed */
  if (win->wm_window == NULL)
    return;
  
  /* If there is no fullscreen window enter fullscreen mode, otherwise exit
     fullscreen mode */
  if (win->fs_window == None)
  {
    XEvent event;
    XSetWindowAttributes setattr;
    /* local width and height vars used for fullscreen window size and for
       storing the video_mode size which is then used to center the window */
    int fs_width  = DisplayWidth(W_Display, W_Screen);
    int fs_height = DisplayHeight(W_Display, W_Screen);

    /* Create the fullscreen window */
    setattr.override_redirect = True;
    setattr.background_pixel = XBlackPixel(W_Display, W_Screen);
    setattr.border_pixel = XBlackPixel(W_Display, W_Screen);
    setattr.event_mask = StructureNotifyMask;
    setattr.colormap = W_Colormap;
    win->fs_window = XCreateWindow(W_Display, W_Root,
                                 0, 0, fs_width, fs_height, 0,
                                 CopyFromParent, InputOutput,
                                 CopyFromParent, CWOverrideRedirect |
                                 CWBackPixel | CWColormap | CWBorderPixel |
                                 CWEventMask, &setattr);

    /* Map the fullscreen window */
    XMapRaised(W_Display, win->fs_window);
    /* wait until we are mapped. (shamelessly borrowed from SDL) */
    do {
      XMaskEvent(W_Display, StructureNotifyMask, &event); 
    } while ( (event.type != MapNotify) || 
              (event.xmap.event != win->fs_window) );
    /* Make sure we got to the top of the window stack */
    XRaiseWindow(W_Display, win->fs_window);
    
    /* Reparent the real window */
    XReparentWindow(W_Display, win->window, win->fs_window, 0, 0);

    /* Grab the keyboard and mouse.  */
    if (XGrabKeyboard(W_Display, win->window, False, GrabModeAsync,
          GrabModeAsync, CurrentTime) != GrabSuccess)
      fprintf(stderr, "Warning can not grab keyboard\n");
    else
      keyboard_grabbed = 1;
    
    if (XGrabPointer(W_Display, win->window, False,
          PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
          GrabModeAsync, GrabModeAsync, win->window, None,
          CurrentTime) != GrabSuccess)
      fprintf(stderr, "Warning can not grab mouse\n");
    else
      mouse_grabbed = 1;

#ifdef XF86VIDMODE      
    /* Try to switch video mode. This must be done after the pointer is
       grabbed, because otherwise it can be outside the window negating the
       XF86VidModeSetViewPort. */
    if (XF86VidModeGetModeLine(W_Display, W_Screen, &i, &mode))
    {
      fs_width  = mode.hdisplay;
      fs_height = mode.vdisplay;
    }
    else   /* If we can't get the currentmode force setting of a new mode. */
      mode.hdisplay = 0;
      
    /* Get list of modelines.  */
    if (!XF86VidModeGetAllModeLines(W_Display, W_Screen, &mode_count, &modes))
            mode_count = 0;
       
    /* Search for an exact matching video mode.  */
    for (i = 0; i < mode_count; i++) {
       if ((modes[i]->hdisplay == win->width) && 
           (modes[i]->vdisplay == win->height))
         break;
    }

    /* Search for a non exact match (smallest bigger res). */
    if (i == mode_count) {
      int best_width = 0, best_height = 0;
      qsort(modes, mode_count, sizeof(void *), cmpmodes);
      for (i = mode_count-1; i > 0; i--) {
         if ( ! best_width ) {
            if ( (modes[i]->hdisplay >= win->width) &&
                 (modes[i]->vdisplay >= win->height) ) {
               best_width = modes[i]->hdisplay;
               best_height = modes[i]->vdisplay;
            }
         } else {
            if ( (modes[i]->hdisplay != best_width) ||
                 (modes[i]->vdisplay != best_height) ) {
               i++;
               break;
            }
         }
      }
    }
        
    /* Switch video mode.  */
    if (((modes[i]->hdisplay != mode.hdisplay) ||
         (modes[i]->vdisplay != mode.vdisplay)) &&
        XF86VidModeSwitchToMode(W_Display, W_Screen, modes[i]))
    {
      fs_width  = modes[i]->hdisplay;
      fs_height = modes[i]->vdisplay;
    }
     
    /* Lock mode switching.  */
    XF86VidModeLockModeSwitch(W_Display, W_Screen, True);
           
    /* Set viewport. */
    XF86VidModeSetViewPort(W_Display, W_Screen, 0, 0);
#endif
    
    /* Center the window (if nescesarry) */
    if ((fs_width != win->width) || (fs_height != win->height))
      XMoveWindow(W_Display, win->window, (fs_width - win->width) / 2,
                   (fs_height - win->height) / 2);
                   
    /* And last blank the cursor */
    W_BlankCursor(_win);
  }
  else
  {
    struct window *wm_window = (struct window *)win->wm_window;
    
    if (mouse_grabbed) {
      XUngrabPointer(W_Display, CurrentTime);
      mouse_grabbed = 0;
    }
    if (keyboard_grabbed) {
      XUngrabKeyboard(W_Display, CurrentTime);
      keyboard_grabbed = 0;
    }

#ifdef XF86VIDMODE
    /* Unlock mode switching.  */
    XF86VidModeLockModeSwitch(W_Display, W_Screen, False);

    if (!XF86VidModeGetModeLine(W_Display, W_Screen, &i, &mode) ||
        (mode.hdisplay != XF86VidModeData.orig_mode.hdisplay) ||
        (mode.vdisplay != XF86VidModeData.orig_mode.vdisplay))
    {
      if (!XF86VidModeSwitchToMode(W_Display, W_Screen,
            &XF86VidModeData.orig_mode))
        fprintf(stderr, "XF86VidMode couldnot restore original resolution\n");
    }
    if (XF86VidModeData.orig_viewport_x || XF86VidModeData.orig_viewport_y)
    {
      if (!XF86VidModeSetViewPort(W_Display, W_Screen,
            XF86VidModeData.orig_viewport_x,
            XF86VidModeData.orig_viewport_y))
        fprintf(stderr, "XF86VidMode couldnot restore original viewport\n");
    }
    XSync(W_Display, False);
#endif
    /* Reparent the real window! */
    XReparentWindow(W_Display, win->window, wm_window->window, 0, 0);
    XUnmapWindow(W_Display, win->fs_window);
    XDestroyWindow(W_Display, win->fs_window);
    win->fs_window = None;

    /* And last unblank the cursor */
    W_RevertCursor(_win);
  }
}
