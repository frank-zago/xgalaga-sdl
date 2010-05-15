#include "allincludes.h"

int
W_Mono()
{
    return forceMono;
}
  
void
W_Beep()
{
    XBell(W_Display, 0);
}

int
W_WindowWidth(window)
    W_Window window;
{
    return (W_Void2Window(window)->width);
}

int
W_WindowHeight(window)
    W_Window window;
{
    return (W_Void2Window(window)->height);
}

int
W_Socket()
{
    return (ConnectionNumber(W_Display));
}

void
W_Flush()
{
    XFlush(W_Display);
}

#ifdef AUTOKEY
void W_AutoRepeatOff()
{
    XAutoRepeatOff(W_Display);
    W_Flush();
}

void W_AutoRepeatOn()
{
    XAutoRepeatOn(W_Display);
    W_Flush();
}
#endif				/* AUTOKEY */

/* synchronize the X server with the client [BDyess] */
void 
W_Sync()
{
  XSync(W_Display,False);
}

