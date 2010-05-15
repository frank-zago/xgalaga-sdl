#ifndef _data_h_
#define _data_h_

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
#include "defs.h"
#include "struct.h"

extern int	nplanes;
extern int	xpmORplanes ;
extern unsigned long	base;
extern unsigned long planes[MAXPLANES];
extern XColor	allocated_colors[1<<MAXPLANES];
extern int	nallocated_colors ;

extern char *_nfonts[];
extern char *_bfonts[];
extern char *_ifonts[];
extern char *_bgfonts[];

extern int zero ;
extern int one ;
extern int two ;
extern int three ;

extern GC maskGC; /* JJJ */

/* GC used for drawing the borders [BDyess] */
extern GC borderGC;
extern int     controlkey ;
extern int     altkey ;
extern int     W_FastClear ;
extern int     buttonDown ;
extern Display *W_Display;
extern Window  W_Root;
extern Colormap W_Colormap;
extern int     W_Screen;
extern Visual *W_Visual;

extern int     W_in_message ;	/* jfy -- for Jerry's warp message hack */

#ifdef RJC
extern W_Window baseWin;
extern XClassHint class_hint;

extern XWMHints wm_hint;

extern XSizeHints wm_size_hint;
#endif				/* RJC */

extern W_Event W_myevent;
extern int W_isEvent ;

extern struct colors colortable[8];

extern struct windowlist *hashtable[HASHSIZE];
extern struct fontInfo fonts[FONTS];

extern struct window myroot;

extern unsigned char gray[];

extern unsigned char striped[];

extern unsigned char solid[];

extern int forceMono;
extern int xpm, useOR, useClipMask;
extern int backColor;

extern char *imagedir;
extern int verbose_image_loading;

extern int useBuffered;

extern unsigned BillsScrewyAltMask;

#endif /* _data_h_ */
