//todo licence

#include <SDL/SDL.h>

#include "data.h"
#include "defs.h"
#include "images.h"
#include "proto.h"

SDL_Surface *screen;

void toggle_fullscreen()
{
	Uint32 flags = SDL_SWSURFACE;
	Uint32 old_flags = screen->flags;

	fullscreen = !fullscreen;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

	screen = SDL_SetVideoMode(0, 0, 0, flags);
	if (!screen)
		screen = SDL_SetVideoMode(0, 0, 0, old_flags);

	if (!screen) {
		fprintf(stderr, "Couldn't toggle screen\n");
		exit(1);
    }
}

void S_Initialize(int fullscreen)
{
	int bpp;
	Uint32 flags = SDL_SWSURFACE;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
    }

    bpp = SDL_VideoModeOK(WINWIDTH, WINHEIGHT, 8, flags);
    if (bpp == 0) {
        fprintf(stderr, "Couldn't get a video mode: %s\n", SDL_GetError());
		exit(1);
    }

    screen = SDL_SetVideoMode(WINWIDTH, WINHEIGHT, bpp, flags);
    if (screen == NULL) {
        fprintf(stderr, "Couldn't get video mode: %s\n", SDL_GetError());
        exit(1);
    }
}

/* Draw a point. Surface must be locked with SDL_LockSurface(). */
void S_DrawPoint(unsigned int x, unsigned int y, Uint32 pixel)
{
	Uint8 *bits, bpp;
	Uint8 r, g, b;

	/* Calculate the framebuffer offset of the center of the screen */
	bpp = screen->format->BytesPerPixel;
	bits = (Uint8 *)screen->pixels + y*screen->pitch + x*bpp;

	/* Set the pixel */
	switch(bpp) {
	case 1:
		*((Uint8 *)(bits)) = pixel;
		break;

	case 2:
		*((Uint16 *)(bits)) = pixel;
		break;

	case 3:
		/* Format/endian independent */
		r = (pixel>>screen->format->Rshift)&0xFF;
		g = (pixel>>screen->format->Gshift)&0xFF;
		b = (pixel>>screen->format->Bshift)&0xFF;
		*((bits)+screen->format->Rshift/8) = r;
		*((bits)+screen->format->Gshift/8) = g;
		*((bits)+screen->format->Bshift/8) = b;
		break;

	case 4:
		*((Uint32 *)(bits)) = pixel;
		break;
	}
	return;
}

void S_ClearScreen(void)
{
    Uint32 color = 0x000000;

    SDL_FillRect (screen, NULL, color); 
}

void S_DrawImage(int x, int y, int frame, struct W_Image *image)
{
	int height, width;
	SDL_Rect srcrect, dstrect;

	if (frame < 0) {
		/* Draw the whole thing regardless of frames. */
		height = image->height * image->frames;
		frame = 0;
	} else {
		/* Draw the given frame. */
		height = image->height;
		frame = frame % image->frames;
	}
	width = image->width;

	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = width;
	dstrect.h = height;

	srcrect.x = 0;
	srcrect.y = height * frame;
	srcrect.w = width;
	srcrect.h = height;

	SDL_BlitSurface (image->surface, &srcrect, screen, &dstrect);
}
