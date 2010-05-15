/* x11window.c
 *
 * Kevin P. Smith  6/11/89
 * Much modified by Jerry Frain and Joe Young
 */

#ifndef _defs_h_
#define _defs_h_

/* define ABORT_ON_ERROR to get a core when X errors occur.  X Buffering is
   also turned off (making display very slow) so the program will stop when
   an error occurs and not later. [BDyess] */
#if 0
#define ABORT_ON_ERROR
#endif /*0*/

#define XPM
#define BUFFERING
#define HACKED_XPM

#define WIN_EDGE 2
#define MENU_PAD 2
#define MENU_BAR 2
#define BORDER 2

#include <stdio.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
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

#define INVALID_POSITION	-10000	/* gotta be a big negative */
/* XFIX speedup */
#define MAXCACHE	128

/* changes too good to risk leaving out, by Richard Caley (rjc@cstr.ed.ac.uk)*/
#define RJC
#define FOURPLANEFIX

#define MAXPLANES	8

#define NORMAL_FONT	"6x10"
#define BOLD_FONT	"6x10"
#define BOLD_FONT2	"-*-clean-bold-r-normal--10-100-75-75-c-60-*"
#define ITALIC_FONT	"6x10"
#define ITALIC_FONT2	"-*-clean-bold-r-normal--10-100-75-75-c-60-*"
#define BIG_FONT	"-adobe-helvetica-bold-r-normal--34-*-*-*-*-*-*-*"
#define IND_FONT        "-*-clean-bold-r-normal--10-100-75-75-c-60-*"

#define FONTS 4
#define BITGC 4
#define BITMASKGC 5
#define NONFONTS 2

#define NONE		-1
#define WHITE   	0
#define BLACK   	1
#define RED     	2
#define GREEN   	3
#define YELLOW  	4
#define CYAN    	5
#define DARK_GREY	6
#define LIGHT_GREY 	7

#define WIN_GRAPH	1
#define WIN_TEXT	2
#define WIN_MENU	3
#define WIN_SCROLL	4
#define WIN_BORDER	5		/* border windows [BDyess] */

#ifndef BUFFERING
#define drawable window
#endif /*BUFFERING [BDyess]*/

#define HASHSIZE 29
#define hash(x) (((int) (x)) % HASHSIZE)

#define NCOLORS (sizeof(colortable)/sizeof(colortable[0]))
#define W_Void2Window(win) ((win) ? (struct window *) (win) : &myroot)
#define W_Window2Void(window) ((W_Window) (window))
#define fontNum(font) (*((int *) font))
#define TILESIDE 16

#ifdef ABORT_ON_ERROR
static int
_myerror(d, e)
    Display *d;
    XErrorEvent *e;
{
    abort();
}
#endif		/* ABORT_ON_ERROR */

#endif /* _defs_h_ */
