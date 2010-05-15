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

#include "Wlib.h"
#include "data.h"
#include "struct.h"

int	nplanes=3;
int	xpmORplanes = 4;
unsigned long	base;
unsigned long planes[MAXPLANES];
XColor	allocated_colors[1<<MAXPLANES];
int	nallocated_colors = -1;

char *_nfonts[] = {
    NORMAL_FONT,
    "-*-clean-medium-r-normal--10-100-75-75-c-60-*",
    "fixed",
    NULL,
};
char *_bfonts[] = {
    BOLD_FONT,
    "-*-clean-bold-r-normal--10-100-75-75-c-60-*",
    "fixed",
    NULL,
};
char *_ifonts[] = {
    ITALIC_FONT,
    "-*-clean-bold-r-normal--10-100-75-75-c-60-*",
    "fixed",
    NULL,
};
char *_bgfonts[] = {
    BIG_FONT,
    "-*-lucidatypewriter-*-*-*-*-40-*-*-*-*-*-*-*",
    "fixed",
    NULL,
};

int zero = 0;
int one = 1;
int two = 2;
int three = 3;

GC maskGC; /* JJJ */

/* GC used for drawing the borders [BDyess] */
GC borderGC;
int     controlkey = 0;
unsigned BillsScrewyAltMask = Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask;
int     altkey = 0;
int     W_FastClear = 0;
#ifdef CONTINUOUS_MOUSE
int     buttonDown = 0;
#endif				/* CONTINUOUS_MOUSE */
Display *W_Display;
Window  W_Root;
Colormap W_Colormap;
int     W_Screen;
#ifdef FOURPLANEFIX
Visual *W_Visual;
#endif
W_Font  W_BigFont = (W_Font) & zero, W_RegularFont = (W_Font) & one;
W_Font  W_HighlightFont = (W_Font) & two, W_UnderlineFont = (W_Font) & three;
W_Color W_White = WHITE, W_Black = BLACK, W_Red = RED, W_Green = GREEN;
W_Color W_Yellow = YELLOW, W_Cyan = CYAN, W_Grey = LIGHT_GREY, W_DarkGrey = DARK_GREY;
int     W_Textwidth, W_Textheight, W_BigTextwidth, W_BigTextheight;
#ifndef strdup
char   *strdup();
#endif
int     W_in_message = 0;	/* jfy -- for Jerry's warp message hack */

#ifdef RJC
extern W_Window baseWin;
 XClassHint class_hint = {
    "xgalaga", "XGalaga",
};

XWMHints wm_hint = {
    InputHint | StateHint,
    True,
    NormalState,
    None,
    None,
    0, 0,
    None,
    None,
};

XSizeHints wm_size_hint;
#endif				/* RJC */

W_Event W_myevent;
int W_isEvent = 0;

struct colors colortable[8] = {
    {"white"},
    {"black"},
    {"red"},
    {"green"},
    {"yellow"},
    {"cyan"},
    {"#ffff00"},	/* dark grey (Actually yellow, but
			   used as a cycling color on 8 bit displays.
			   yellow so it stands out on mono/truecolor.*/
    {"#c8c8c8"}		/* light grey */
};

struct windowlist *hashtable[HASHSIZE];
struct fontInfo fonts[FONTS];

struct window myroot;

unsigned char gray[] = {
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55
};

unsigned char striped[] = {
    0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
    0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f,
    0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00,
    0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0
};

unsigned char solid[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

int forceMono = 0;

int xpm = 1, useOR = 1, useClipMask = 1;

int backColor;

char *imagedir= 0;

int verbose_image_loading = 0;

int useBuffered = 1;

#ifdef XF86VIDMODE
struct xf86vidmode_data XF86VidModeData = { 1, { 0 }, 0, 0 };
#endif
