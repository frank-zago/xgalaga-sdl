/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
 * Copyright (c) 2000 Andy Tarkinson <atark@thepipeline.net>
 * Copyright (c) 2010,2011 Frank Zago
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "xgalaga.h"

#if SDL_VERSION_ATLEAST(1,3,0)
SDL_Window *screen;
SDL_Renderer *renderer;
#else
static Uint32 background_color;
SDL_Surface *screen;
#endif

void toggle_fullscreen(void)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	const int screen_height = winheight+WINTOPOV+WINBOTOV;
    Uint32 flags = SDL_GetWindowFlags(screen) ^ SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (SDL_SetWindowFullscreen(screen, flags) < 0)
        return;

    SDL_SetWindowSize(screen, winwidth, screen_height);
#else
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
#endif
}

void S_Initialize(int fullscreen)
{
	const int screen_height = winheight+WINTOPOV+WINBOTOV;

#if SDL_VERSION_ATLEAST(1,3,0)
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Couldn't init video: %s\n", SDL_GetError());
        exit(1);
    }

	atexit(SDL_Quit);

	screen = SDL_CreateWindow("Xgalaga SDL",
							  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							  winwidth, screen_height,
							  SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!screen) {
        fprintf(stderr, "Couldn't set %dx%d video mode: %s\n",
                winwidth, screen_height, SDL_GetError());
        exit(1);
    }

	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		fprintf(stderr, "Couldn't find a renderer\n");
        exit(1);
	}

	if (fullscreen)
		SDL_RenderSetLogicalSize(renderer, winwidth, screen_height);
#else

	int bpp;
	Uint32 flags = SDL_SWSURFACE;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
    }

	atexit(SDL_Quit);

    bpp = SDL_VideoModeOK(winwidth, screen_height, 8, flags);
    if (bpp == 0) {
        fprintf(stderr, "Couldn't get a video mode1: %s\n", SDL_GetError());
		exit(1);
    }

    screen = SDL_SetVideoMode(winwidth, screen_height, bpp, flags);
    if (screen == NULL) {
        fprintf(stderr, "Couldn't get video mode2: %s\n", SDL_GetError());
        exit(1);
    }

	background_color = SDL_MapRGBA(screen->format, 0, 0, 0, 255);
#endif

	SDL_ShowCursor(SDL_DISABLE);
}

/* Draw a point. Surface must be locked with SDL_LockSurface(). */
void S_DrawPoint(unsigned int x, unsigned int y, struct color color)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawPoint(renderer, x, y + WINTOPOV);
#else
	Uint8 *bits, bpp;
	Uint8 r, g, b;

	y+= WINTOPOV;

	/* Calculate the framebuffer offset of the center of the screen */
	bpp = screen->format->BytesPerPixel;
	bits = (Uint8 *)screen->pixels + y*screen->pitch + x*bpp;

	/* Set the pixel */
	switch(bpp) {
	case 1:
		*((Uint8 *)(bits)) = color.rgb;
		break;

	case 2:
		*((Uint16 *)(bits)) = color.rgb;
		break;

	case 3:
		/* Format/endian independent */
		r = (color.rgb>>screen->format->Rshift)&0xFF;
		g = (color.rgb>>screen->format->Gshift)&0xFF;
		b = (color.rgb>>screen->format->Bshift)&0xFF;
		*((bits)+screen->format->Rshift/8) = r;
		*((bits)+screen->format->Gshift/8) = g;
		*((bits)+screen->format->Bshift/8) = b;
		break;

	case 4:
		*((Uint32 *)(bits)) = color.rgb;
		break;
	}
#endif

	return;
}

void S_ClearScreen(void)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
#else
    SDL_FillRect (screen, NULL, background_color);
#endif

}

void S_UpdateDisplay(void)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_RenderPresent(renderer);
#else
	SDL_Flip(screen);
#endif
}

void S_DrawImage(int x, int y, int frame, struct W_Image *image)
{
	int height, width;
	SDL_Rect srcrect, dstrect;

	y+= WINTOPOV;

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

#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_RenderCopy(renderer, image->surface, &srcrect, &dstrect);
#else
	SDL_BlitSurface (image->surface, &srcrect, screen, &dstrect);
#endif
}

void S_DrawRect(int x, int y, int w, int h, struct  color color)
{
    SDL_Rect dstrect;

	y+= WINTOPOV;

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = w;
    dstrect.h = h;
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(renderer, &dstrect);
#else
    SDL_FillRect(screen, &dstrect, color.rgb);
#endif
}
