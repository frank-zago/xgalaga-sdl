/* border.c */
void W_ChangeBorder(W_Window window, int color);
void redrawBorder(struct window *win);
/* cursor.c */
Cursor make_cursor(char *bits, char *mask, unsigned int width, unsigned int height, unsigned int xhot, unsigned int yhot);
void W_DefineTCrossCursor(W_Window window);
void W_DefineTextCursor(W_Window window);
void W_RevertCursor(W_Window window);
void W_DefineCursor(W_Window window, int width, int height, char *bits, char *mask, int xhot, int yhot);
void W_BlankCursor(W_Window window);
/* init.c */
void W_Initialize(char *str);
/* misc.c */
int W_Mono(void);
void W_Beep(void);
int W_WindowWidth(W_Window window);
int W_WindowHeight(W_Window window);
int W_Socket(void);
void W_Flush(void);
void W_AutoRepeatOff(void);
void W_AutoRepeatOn(void);
void W_Sync(void);
/* sector.c */
void W_DrawSectorHighlight(W_Window window, int x, int y, int width, int h, W_Color color);
/* buffer.c */
void W_ClearBuffer(W_Window window);
int W_IsBuffered(W_Window window);
void W_Buffer(W_Window window, int on);
void W_DisplayBuffer(W_Window window);
/* data.c */
/* line.c */
void FlushLineCache(Window win, int color);
void W_CacheLine(W_Window window, int X0, int Y0, int X1, int Y1, int color);
void W_FlushLineCaches(W_Window window);
void W_MakeLine(W_Window window, int X0, int Y0, int X1, int Y1, W_Color color);
void W_MakeTractLine(W_Window window, int X0, int Y0, int X1, int Y1, W_Color color);
/* mouse.c */
void W_WarpPointer(W_Window window, int x, int y);
int findMouseInWin(int *x, int *y, W_Window window);
void W_GetMouse(W_Window window, int *x, int *y, int *but);
void W_GrabPointer(W_Window window);
void W_UngrabPointer(void);
/* text.c */
void W_ShadowText(W_Window window, int x, int y, W_Color color, char *str, int len, W_Font font);
void W_WriteText(W_Window window, int x, int y, W_Color color, char *str, int len, W_Font font);
void W_MaskText(W_Window window, int x, int y, W_Color color, char *str, int len, W_Font font);
void W_DirectMaskText(W_Window window, int x, int y, W_Color color, char *str, int len, W_Font font);
int W_StringWidth(char string[], W_Font font);
void W_ResizeText(W_Window window, int neww, int newh);
/* circle.c */
void W_DrawCircle(W_Window window, int centerx, int centery, unsigned int diameter, W_Color color);
void W_WriteArc(int filled, W_Window window, int x, int y, int width, int height, int angle1, int angle2, W_Color color);
/* event.c */
void W_GetEvent(W_Event *wevent);
int W_EventsPending(void);
void W_NextEvent(W_Event *wevent);
int W_SpNextEvent(W_Event *wevent);
/* makewindow.c */
struct window *newWindow(Window window, int type);
void W_RenameWindow(W_Window window, char *str);
W_Window w_MakeWindow(char *name, int x, int y, int width, int height, W_Window parent, char *cursname, int border, W_Color color, int wsort);
W_Window W_MakeWindow(char *name, int x, int y, int width, int height, W_Window parent, char *cursname, int border, W_Color color);
W_Window W_MakeTextWindow(char *name, int x, int y, int width, int height, W_Window parent, char *cursname, int border);
W_Window W_MakeScrollingWindow(char *name, int x, int y, int width, int height, W_Window parent, char *cursname, int border);
void W_SetIconWindow(W_Window win, W_Window icon);
void W_ToggleFullscreen(W_Window win);
/* point.c */
void W_DrawPoint(W_Window window, int x, int y, W_Color color);
void FlushPointCache(Window win, int color);
void W_CachePoint(W_Window window, int x, int y, int color);
void W_FlushPointCaches(W_Window window);
/* tile.c */
void W_TileWindow(W_Window window, W_Image *image);
void W_UnTileWindow(W_Window window);
/* cleararea.c */
void FlushClearAreaCache(W_Window window);
void W_CacheClearArea(W_Window window, int x, int y, int width, int height);
void W_FlushClearAreaCache(W_Window window);
void W_ClearArea(W_Window window, int x, int y, unsigned int width, unsigned int height);
void W_ClearWindow(W_Window window);
/* fill.c */
void W_FillArea(W_Window window, int x, int y, unsigned int width, unsigned int height, W_Color color);
/* mapwindow.c */
void W_MapWindow(W_Window window);
void W_UnmapWindow(W_Window window);
int W_IsMapped(W_Window window);
void W_DestroyWindow(W_Window window);
void W_ResizeWindow(W_Window window, int neww, int newh);
void W_Deiconify(W_Window window);
/* scale.c */
void W_TranslatePoints(W_Window window, int *x, int *y);
/* triangle.c */
void W_WriteAnyTriangle(W_Window window, int X1, int Y1, int X2, int Y2, int X3, int Y3, W_Color color);
void W_WriteTriangle(W_Window window, int x, int y, int s, int t, W_Color color);
/* color.c */
void W_SetRGB16(W_Color color, int r, int g, int b);
/* image.c */
void W_SetImageDir(char *dir);
void W_FreeImage(W_Image *image);
W_Image *W_BitmapToImage(unsigned int width, unsigned int height, char *bits);
void check_loaded(W_Image *image);
W_Image *W_CreateCombinedImage(W_Image **imagelist, W_Color color);
int W_LoadImage(W_Image *image);
void W_DrawImageNoClip(W_Window window, int x, int y, int frame, W_Image *image, W_Color color);
void W_OverlayImage(W_Window window, int x, int y, int frame, W_Image *image, int overframe, W_Image *overimage, W_Color color);
void W_DrawImage(W_Window window, int x, int y, int frame, W_Image *image, W_Color color);
void W_DrawImageOr(W_Window window, int x, int y, int frame, W_Image *image, W_Color color);
void W_DrawImageBar(W_Window win, int x, int y, int len, W_Image *image);
/* menu.c */
W_Window W_MakeMenu(char *name, int x, int y, int width, int height, W_Window parent, int border);
void redrawMenu(struct window *win);
void changeMenuItem(struct window *win, int n, W_Color color, char *str, int len, W_Font font);
void W_ResizeMenu(W_Window window, int neww, int newh);
/* scroll.c */
void AddToScrolling(struct window *win, W_Color color, char *str, int len);
void redrawScrolling(struct window *win);
void W_SetSensitive(W_Window window, int v);
void resizeScrolling(struct window *win, int width, int height);
