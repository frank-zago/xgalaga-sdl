/* Wlib.h
 *
 * Include file for the Windowing interface.
 *
 * Kevin P. Smith  6/11/89
 *
 * The deal is this:
 *   Call W_Initialize(), and then you may call any of the listed fuinctions.
 *   Also, externals you are allowed to pass or use include W_BigFont,
 *     W_RegularFont, W_UnderlineFont, W_HighlightFont, W_White, W_Black,
 *     W_Red, W_Green, W_Yellow, W_Cyan, W_Grey, W_Textwidth, and W_Textheight.
 */

#ifndef Wlib_h_
#define Wlib_h_

#define AUTOKEY

#include <X11/Xlib.h>
#include "copyright2.h"

struct window;

#ifdef __STDC__
#define P(s) s
#else
#define P(s) ()
#endif

/* image struct to hold all info about an image, be it a bitmap or 
   pixmap.  Replaces W_Icon.  [BDyess] */
typedef struct {
  /* public */
  unsigned int width, height, frames;	/* frames = nviews for ships */
  int xpm;			/* is it a Pixmap (> 1 plane) or not */
  char *filename;		/* filename without .xpm/.xbm extension*/
  int loaded;			/* 1 if loaded (for on-demand loading) */
  int alternate;		/* offset into images array for alternate
                                   image to use if this one can't be
				   loaded */
  int bad;			/* if set, loading failed */
  int compiled_in;		/* image is compiled into the binary */
  unsigned char *xbmdata;	/* ptr to compiled in xbm data.  Only valid */
                                /* if compiled_in is true. */
  char **xpmdata;		/* ptr to compiled in xpm data.  Only valid */
                                /* if compiled_in is true. */
  /* X Data structures (private) */
  Pixmap pixmap, clipmask;	/* clipmask only used if in xpm mode */
} W_Image;

/*typedef char *W_Window;*/
typedef char *W_Icon;
typedef char *W_Font;
typedef int W_Color;

typedef int (*W_Callback) ();
typedef char *W_Window;

extern W_Font W_BigFont, W_RegularFont, W_UnderlineFont, W_HighlightFont;
extern W_Color W_White, W_Black, W_Red, W_Green, W_Yellow, W_Cyan, W_Grey, W_DarkGrey;
extern int W_Textwidth, W_Textheight, W_BigTextwidth, W_BigTextheight;
extern int W_FastClear;

#define W_EV_EXPOSE	1
#define W_EV_KEY	2
#define W_EV_BUTTON	3

#ifdef AUTOKEY
#define W_EV_KEY_OFF	4
#endif				/* AUTOKEY */

#define W_LBUTTON	1
#define W_MBUTTON	2
#define W_RBUTTON	3

typedef struct event {
    int     type;
    W_Window Window;
    int     key;
    int     x, y;
}       W_Event;

#define W_BoldFont W_HighlightFont

/*#define W_StringWidth(st,font) (strlen(st)*W_Textwidth)*/

#if 0
/* Don't even know what this is supposed to do... */
#define W_SetSensitive(w,b)
#endif /*0*/

extern int autoPoint, W_KeyDepth, animPointers, useWorkbench;
extern unsigned long w_socket;

/* For XF86VIDMODE Support */
#ifdef XF86VIDMODE
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>   

struct xf86vidmode_data {
  int vidmode_available;
  XF86VidModeModeInfo orig_mode;
  int orig_viewport_x;
  int orig_viewport_y;
};

extern struct xf86vidmode_data XF86VidModeData;
#endif

#endif	/* AMIGA */

#if 0
void W_FreeImage P((W_Image *image));
W_Image * W_BitmapToImage P((unsigned int width, unsigned int height, char *bits));
void W_DrawImage P(( W_Window window, int x, int y, int frame, W_Image *image, 
	W_Color color));

extern void W_WritePixmap P((W_Window window, int x, int y, int frame,
	W_Icon bit));
extern W_Icon W_StorePixmap P((char *filename, int frames));

void W_ClearBuffer P((W_Window window));
int W_IsBuffered P((W_Window window));
void W_Buffer P((W_Window window, int on));
void W_DisplayBuffer P((W_Window window));

void W_DrawImageBar P((W_Window win, int x, int y, int len, W_Image *image));
void W_FillArea P((W_Window window, int x, int y, unsigned int width, unsigned int height, 
	W_Color color));
extern void W_GetPixmapInfo P((W_Icon pixmap, int *width, int *height, 
	int *frames));
extern void W_DrawShield P((W_Window window, int centerx, int centery, 
	int diameter, W_Color color));
extern void W_Initialize P((char *display));
extern W_Window W_MakeWindow P((char *name, int x, int y, int width, 
	int height, W_Window parent, char *cursname, int border, 
	W_Color color));
extern W_Icon W_StoreBitmap P((int width, int height, char *data));
extern void W_FreeBitmap P((W_Icon bit));
extern W_Window W_MakeTextWindow P((char *name, int x, int y, int width, 
	int height, W_Window parent, char *cursname, int border));
extern W_Window W_MakeScrollingWindow P((char *name, int x, int y, int width,
	int height, W_Window parent, char *cursname, int border));
extern W_Window W_MakeMenu P((char *name, int x, int y, int width, int height,
	W_Window parent, int border));
extern void W_WriteText P((W_Window window, int x, int y, W_Color color,
	char *str, int len, W_Font font));
void W_MaskText P((W_Window window, int x, int y, W_Color color,
	char *str, int len, W_Font font));
extern void W_DirectMaskText P((W_Window window, int x, int y, W_Color color,
	char *str, int len, W_Font font));
extern void W_WriteBitmap P((W_Window win, int x, int y, W_Icon icon, W_Color color));
extern void W_CacheClearArea();

extern void W_ClearArea( /* window, x, y, width, height, color */ );
extern void W_MakeLine( /* window, x0, y0, x1, y1, color */ );
extern void W_DrawPoint( /* window, x, y, color */ );
void W_MapWindow( /* window */ );
extern void W_UnmapWindow( /* window */ );
extern int W_EventsPending( /* void */ );
extern void W_NextEvent( /* W_Event *event */ );
extern void W_TileWindow( /* window, bitmap */ );
extern void W_UnTileWindow( /* window */ );
extern void W_ChangeBorder( /* window, color */ );
extern void 
W_DefineCursor(	/* window, width, height, data, mask, xhot, yhot */ );
extern int W_IsMapped( /* window */ );
extern void W_Beep( /* void */ );
extern void W_DestroyWindow( /* window */ );
extern int W_WindowWidth( /* window */ );
extern int W_WindowHeight( /* window */ );
extern int W_Socket( /* void */ );
extern void W_ClearWindow( /* window */ );
extern void W_SetIconWindow( /* mainwindow, iconwindow */ );
extern int W_StringWidth( /* string, font */ );
extern void W_TranslatePoints( /* window, x, y */ );
extern void W_ResizeWindow( /* window, neww, newh */ );
extern void W_ResizeMenu( /* window, neww, newh */ );
extern void W_ResizeText( /* window, neww, newh */ );
extern void W_Deiconify( /* window */ );
extern void W_GetMouse();
extern void W_BlankCursor();
extern void W_Flush();

extern W_Icon W_MakeShieldBitmap( /* width,height, window */ );

extern W_Image *W_CreateCombinedImage();

#endif

#include "Wproto.h"

#ifdef AMIGA
void switchScreen();
/* things ignored or different for now: */
/* some laziness on my part, mostly just unneeded. */
#define W_CacheLine W_MakeLine
#define W_CachePoint W_DrawPoint
#define W_FlushPointCaches(w)
#define W_FlushLineCaches(w)
#define W_DefineTextCursor(w) W_DefineMessageCursor(w)
#define W_RevertCursor(w) W_DefineTCrossCursor(w)
#define W_RenameWindow(w,s)

extern W_Icon W_ReadPixmap(W_Window window, char *path, char *name, int width, int height, int x, int y, int mask);

#endif
