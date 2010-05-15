//todo : licence

#include <SDL/SDL_framerate.h>

#include "defs.h"

static FPSmanager manager;

void init_framerate(void)
{
	SDL_initFramerate(&manager);
	SDL_setFramerate(&manager, FPS);
}

void do_framerate(void)
{
	SDL_framerateDelay(&manager);
}
