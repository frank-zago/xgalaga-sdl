#include "allincludes.h"

#ifdef FOURPLANEFIX
static unsigned short extrared[8] = {0x00, 0x20, 0x40, 0x60, 0x80, 0xa0, 0xb0, 0xc0};
static unsigned short extragreen[8] = {0x40, 0x60, 0x80, 0xa0, 0xb0, 0xc0, 0x00, 0x20};
static unsigned short extrablue[8] = {0x80, 0xa0, 0xb0, 0xc0, 0x00, 0x20, 0x40, 0x60};
#endif

static XFontStruct *
find_font(oldf, fnts)
    char   *oldf, **fnts;
{
    XFontStruct *fi;
    char  **f;
    fprintf(stderr, "xgalaga: Can't find font %s.  Trying others...\n",
	    oldf);
    for (f = fnts; *f; f++) {
	if (strcmp(*f, oldf) != 0) {
	    if ((fi = XLoadQueryFont(W_Display, *f)))
		return fi;
	}
    }
    printf("Error - can't find any font!\n");
    return NULL;
}

static void
GetFonts()
{
    Font    regular, italic, bold, big;
    int     i;
    XGCValues values;
    XFontStruct *fontinfo;
    char   *fontname;

    fontname = NORMAL_FONT;
    fontinfo = XLoadQueryFont(W_Display, fontname);
    if (fontinfo == NULL) {
	fontinfo = find_font(fontname, _nfonts);
    }
    if (fontinfo == NULL) {
	printf("xgalaga: Can't find any fonts!\n");
	exit(1);
    }
    regular = fontinfo->fid;
    W_Textwidth = fontinfo->max_bounds.width;
    W_Textheight = fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent;
    fonts[1].baseline = fontinfo->max_bounds.ascent;
    fonts[1].fontstruct = fontinfo;

    if (DisplayCells(W_Display, W_Screen) <= 4)
	fontname = BOLD_FONT2;
    else
	fontname = BOLD_FONT;
    fontinfo = XLoadQueryFont(W_Display, fontname);
    if (fontinfo == NULL) {
	fontinfo = find_font(fontname, _bfonts);
    }
    if (fontinfo == NULL) {
	bold = regular;
	fonts[2].baseline = fonts[1].baseline;
	fonts[2].fontstruct = fonts[1].fontstruct;
    } else {
	bold = fontinfo->fid;
	fonts[2].baseline = fontinfo->max_bounds.ascent;
	fonts[2].fontstruct = fontinfo;
	if (fontinfo->max_bounds.width > W_Textwidth)
	    W_Textwidth = fontinfo->max_bounds.width;
	if (fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent > W_Textheight)
	    W_Textheight = fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent;
    }

    if (DisplayCells(W_Display, W_Screen) <= 4)
	fontname = ITALIC_FONT2;
    else
	fontname = ITALIC_FONT;
    fontinfo = XLoadQueryFont(W_Display, fontname);
    if (fontinfo == NULL) {
	fontinfo = find_font(fontname, _ifonts);
    }
    if (fontinfo == NULL) {
	italic = regular;
	fonts[3].baseline = fonts[1].baseline;
	fonts[3].fontstruct = fonts[1].fontstruct;
    } else {
	italic = fontinfo->fid;
	fonts[3].baseline = fontinfo->max_bounds.ascent;
	fonts[3].fontstruct = fontinfo;
	if (fontinfo->max_bounds.width > W_Textwidth)
	    W_Textwidth = fontinfo->max_bounds.width;
	if (fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent > W_Textheight)
	    W_Textheight = fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent;
    }

    fontname = BIG_FONT;
    fontinfo = XLoadQueryFont(W_Display, fontname);
    if (fontinfo == NULL) {
	fontinfo = find_font(fontname, _bgfonts);
    }
    if (fontinfo == NULL) {
	big = regular;
	fonts[0].baseline = fonts[1].baseline;
	fonts[0].fontstruct = fonts[1].fontstruct;
    } else {
	big = fontinfo->fid;
	fonts[0].baseline = fontinfo->max_bounds.ascent;
	fonts[0].fontstruct = fontinfo;
	W_BigTextwidth = fontinfo->max_bounds.width;
	W_BigTextheight = fontinfo->max_bounds.descent + fontinfo->max_bounds.ascent;
    }
    for (i = 0; i < NCOLORS; i++) {
	values.font = big;
	colortable[i].contexts[0] = XCreateGC(W_Display, W_Root, GCFont, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[0], False);
	values.font = regular;
	colortable[i].contexts[1] = XCreateGC(W_Display, W_Root, GCFont, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[1], False);
	values.font = bold;
	colortable[i].contexts[2] = XCreateGC(W_Display, W_Root, GCFont, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[2], False);
	values.font = italic;
	colortable[i].contexts[3] = XCreateGC(W_Display, W_Root, GCFont, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[3], False);
	{
	    static char dl[] = {1, 4};
	    XSetLineAttributes(W_Display, colortable[i].contexts[3],
			       0, LineOnOffDash, CapButt, JoinMiter);
	    XSetDashes(W_Display, colortable[i].contexts[3], 0, dl, 2);
	}
	values.function = GXor;
	colortable[i].contexts[BITGC] = XCreateGC(W_Display, W_Root, GCFunction, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[BITGC], False);
	colortable[i].contexts[BITMASKGC] = XCreateGC(W_Display, W_Root, 0, &values);
	XSetGraphicsExposures(W_Display, colortable[i].contexts[BITMASKGC], 
			      False);
    }
    values.fill_style = FillSolid;
    values.line_style = LineSolid;
    borderGC = XCreateGC(W_Display, W_Root, GCFillStyle|GCLineStyle, &values);

    values.function = GXand;
    maskGC = XCreateGC(W_Display, W_Root,
		GCFunction , &values);
}

static void
GetColors()
{
    int     i, j;
    XColor  foo;
    int     white, black;
/*    unsigned long pixel;
    unsigned long planes[3];*/
    char    defaultstring[100];
    char   *defs;
#ifdef FOURPLANEFIX
    unsigned long extracolors[8];
    XColor  colordef;
#endif
    extern int forceMono;
    forceMono = 0;

    if ((DisplayCells(W_Display, W_Screen) <= 4) || forceMono) {
	forceMono = 1;
	white = WhitePixel(W_Display, W_Screen);
	black = BlackPixel(W_Display, W_Screen);
	for (i = 0; i < NCOLORS; i++) {
	    if (i != W_Black) {
		colortable[i].pixelValue = white;
	    } else {
		colortable[i].pixelValue = black;
	    }
	    if (i == W_Red) {
		colortable[i].pixmap = XCreatePixmapFromBitmapData
		    (W_Display,
		     W_Root, (char *) striped, TILESIDE, TILESIDE,
		     (unsigned)white, (unsigned)black,
		     (unsigned)DefaultDepth(W_Display, W_Screen));
	    } else if (i == W_Yellow) {
		colortable[i].pixmap = XCreatePixmapFromBitmapData
		    (W_Display,
		     W_Root, (char *) gray, TILESIDE, TILESIDE,
		     (unsigned)white, (unsigned)black,
		     (unsigned)DefaultDepth(W_Display, W_Screen));
	    } else {
		colortable[i].pixmap = XCreatePixmapFromBitmapData
		    (W_Display,
		     W_Root, (char *) solid, TILESIDE, TILESIDE,
		     (unsigned)colortable[i].pixelValue,
		     (unsigned)colortable[i].pixelValue,
		     (unsigned)DefaultDepth(W_Display, W_Screen));
	    }

	    /*
	       We assume white is 0 or 1, and black is 0 or 1. We adjust
	       graphics function based upon who is who.
	    */
	    if (white == 0) {	/* Black is 1 */
		XSetFunction(W_Display, colortable[i].contexts[BITGC], GXand);
	    }
	}
    } else if (DefaultVisual(W_Display, W_Screen)->class == TrueColor) {
/* Stuff added by sheldon@iastate.edu 5/28/93
 * This is supposed to detect a TrueColor display, and then do a lookup of
 * the colors in default colormap, instead of creating new colormap
 */
	for (i = 0; i < NCOLORS; i++) {
	    sprintf(defaultstring, "color.%s", colortable[i].name);

/*	    defs = stringDefault(defaultstring,NULL);
	    if (defs == NULL)
*/
	        defs = colortable[i].name;
	    XParseColor(W_Display, W_Colormap, defs, &foo);
	    XAllocColor(W_Display, W_Colormap, &foo);
	    colortable[i].pixelValue = foo.pixel;
	    colortable[i].pixmap = XCreatePixmapFromBitmapData(W_Display,
	    W_Root, (char *) solid, TILESIDE, TILESIDE, (unsigned)foo.pixel, 
	    (unsigned)foo.pixel, (unsigned)DefaultDepth(W_Display, W_Screen));
	}
    } else {
#ifdef FOURPLANEFIX
#ifndef HACKED_XPMLIB
	xpmORplanes = DefaultDepth(W_Display,W_Screen);
#endif /*HACKED_XPMLIB*/
        nplanes = (xpm && useOR) ? xpmORplanes : 8;
	nplanes = 7;
	if (!XAllocColorCells(W_Display, W_Colormap, False, planes, nplanes,
			      &base, 1)) {
	    /* couldn't allocate (nplanes) planes, make a new colormap */
	    printf("Couldn't allocate %d planes for private use, so allocating a private colormap.\n",nplanes);
	    W_Colormap = XCreateColormap(W_Display, W_Root, W_Visual, AllocNone);
	    if (!XAllocColorCells(W_Display, W_Colormap, False, planes, 3/*nplanes*/,
				  &base, 1)) {
		fprintf(stderr, "Cannot create new colormap\n");
		exit(1);
	    }
	    /*
	       and fill it with at least 8 more colors so when mouse is
	       inside xGalaga windows, use might be able to see his other
	       windows
	    */
	    if (XAllocColorCells(W_Display, W_Colormap, False, NULL, 0,
				 extracolors, 8)) {
		colordef.flags = DoRed | DoGreen | DoBlue;
		for (i = 0; i < 8; i++) {
		    colordef.pixel = extracolors[i];
		    colordef.red = extrared[i] << 8;
		    colordef.green = extragreen[i] << 8;
		    colordef.blue = extrablue[i] << 8;
		    XStoreColor(W_Display, W_Colormap, &colordef);
		}
	    }
	}
#else
	XAllocColorCells(W_Display, W_Colormap, False, planes, nplanes, &base, 1);
#endif
	nallocated_colors = NCOLORS + 1;
	for (i = 0; i < NCOLORS; i++) {
	    /*
	       strcpy(defaultstring, "color.%s", colortable[i].name);
	    */
	    sprintf(defaultstring, "color.%s", colortable[i].name);

	    defs = strdup(colortable[i].name);
	    XParseColor(W_Display, W_Colormap, defs, &foo);
	    /*
	       Black must be the color with all the planes off. That is the
	       only restriction I concerned myself with in the following case
	       statement.
	    */
	    switch (i) {
	    case WHITE:
		foo.pixel = base | planes[0] | planes[1] | planes[2];
		memcpy(&allocated_colors[7], &foo, sizeof(foo));
		break;
	    case BLACK:
		foo.pixel = base;
		memcpy(&allocated_colors[0], &foo, sizeof(foo));
		break;
	    case RED:
		foo.pixel = base | planes[0];
		memcpy(&allocated_colors[1], &foo, sizeof(foo));
		break;
	    case CYAN:
		foo.pixel = base |             planes[1];
		memcpy(&allocated_colors[2], &foo, sizeof(foo));
		break;
	    case YELLOW:
		foo.pixel = base |                         planes[2];
		memcpy(&allocated_colors[4], &foo, sizeof(foo));
		break;
	    case DARK_GREY:
		foo.pixel = base | planes[0] | planes[1];
		memcpy(&allocated_colors[3], &foo, sizeof(foo));
		break;
	    case GREEN:
		foo.pixel = base |             planes[1] | planes[2];
		memcpy(&allocated_colors[6], &foo, sizeof(foo));
		break;
	    case LIGHT_GREY:
	        foo.pixel = base | planes[0] |             planes[2];
		memcpy(&allocated_colors[5], &foo, sizeof(foo));
                break;
	    }
	    XStoreColor(W_Display, W_Colormap, &foo);
	    colortable[i].pixelValue = foo.pixel;
	    colortable[i].pixmap = XCreatePixmapFromBitmapData(W_Display,
	    W_Root, (char *) solid, TILESIDE, TILESIDE, foo.pixel, foo.pixel,
		(unsigned) DefaultDepth(W_Display, W_Screen));
	}
    }
    for (i = 0; i < NCOLORS; i++) {
	for (j = 0; j < FONTS + NONFONTS; j++) {
	    XSetForeground(W_Display, colortable[i].contexts[j],
			   colortable[i].pixelValue);
	    XSetBackground(W_Display, colortable[i].contexts[j],
			   colortable[W_Black].pixelValue);
	}
    }
    /* set FG/BG for border GC [BDyess] */
    XSetBackground(W_Display, borderGC, colortable[DARK_GREY].pixelValue);
    XSetForeground(W_Display, borderGC, colortable[LIGHT_GREY].pixelValue);

    XSetBackground(W_Display, maskGC, 0xffffffff);
    XSetForeground(W_Display, maskGC, 0);
}

/* Any of Mod1 through Mod5 that are generated by XK_Num_Lock or
 * XK_Scroll_Lock are excluded from BillsScrewyAltMask.
 */
static void
GetAltMask()
{
    XModifierKeymap *m;
    int     i, j;
    KeySym  k;

    m = XGetModifierMapping(W_Display);
    if (m == NULL) {
        printf("Not enough memory\n");
        exit(1);
    }

    for (i = 3; i < 8; i++) {
        for (j = 0; j < m->max_keypermod; j++) {
            k = XKeycodeToKeysym(W_Display,
                m->modifiermap [i*m->max_keypermod + j], 0);
            if (k == XK_Num_Lock || k == XK_Scroll_Lock) {
                BillsScrewyAltMask &= ~(1 << i);
                break;
            }
        }
    }

    XFreeModifiermap(m);
}

#ifdef XF86VIDMODE
static void
xf86_vidmode_init ( void )
{
  int i,j;
  XF86VidModeModeLine *l = (XF86VidModeModeLine *)((char *)
    &XF86VidModeData.orig_mode + sizeof XF86VidModeData.orig_mode.dotclock);
  
  if (!XF86VidModeQueryVersion(W_Display, &i, &j))
    XF86VidModeData.vidmode_available = 0;
  else if (!XF86VidModeQueryExtension(W_Display, &i, &j))
    XF86VidModeData.vidmode_available = 0;
  else if (!XF86VidModeGetModeLine(W_Display, W_Screen,
            (int *)&XF86VidModeData.orig_mode.dotclock, l))
    XF86VidModeData.vidmode_available = 0;
  else if (!XF86VidModeGetViewPort(W_Display, W_Screen,
            &XF86VidModeData.orig_viewport_x,
            &XF86VidModeData.orig_viewport_y))
    XF86VidModeData.vidmode_available = 0;
  
  if (!XF86VidModeData.vidmode_available)
    fprintf(stderr, "Warning: XF86VidMode not available\n");
}
#endif

void
W_Initialize(str)
    char   *str;
{
    int     i;

#ifdef DEBUG
    printf("Initializing...\n");
#endif
    for (i = 0; i < HASHSIZE; i++) {
	hashtable[i] = NULL;
    }
    if ((W_Display = XOpenDisplay(str)) == NULL) {
	fprintf(stderr, "Cannot open display \"%s\"\n", str ? str : "(null)");
	exit(1);
    }
#ifdef ABORT_ON_ERROR
    /* tmp */
    XSynchronize(W_Display, True);
    XSetErrorHandler(_myerror);
#endif

    W_Root = DefaultRootWindow(W_Display);
#ifdef FOURPLANEFIX
    W_Visual = DefaultVisual(W_Display, DefaultScreen(W_Display));
#endif
    W_Screen = DefaultScreen(W_Display);
    W_Colormap = DefaultColormap(W_Display, W_Screen);
    myroot.window = W_Root;
    myroot.type = WIN_GRAPH;
/*
    imagedir = strdup(IMAGEDIR);
    if(verbose_image_loading) printf("imagedir = %s\n",imagedir);
*/
    GetFonts();
    GetColors();
    GetAltMask();

    backColor = W_Black;
    
#ifdef XF86VIDMODE
    xf86_vidmode_init();
#endif
}

