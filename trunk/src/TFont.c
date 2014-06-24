/*  TFont: On systems that support TTF FONTS, this creates font images
    instead of using font images in SFont.c
    Copyright (C) 2011 Dan Espen
    Copyright (C) 2010,2011 Frank Zago
    Copyright (C) 2003 Karl Bartel

    License: GPL or LGPL (at your choice)
    WWW: http://www.linux-games.com/sfont/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Karl Bartel
    Cecilienstr. 14
    12307 Berlin
    GERMANY
    karlb@gmx.net
*/

/* If NOT using TTF Fonts, this file does nothing,
   check Sfont.c. */
#if defined HAVE_LIBSDL_TTF || defined HAVE_LIBSDL2_TTF
#include <assert.h>

#include "xgalaga.h"

#include <sys/param.h>
#include <SDL_ttf.h>

static const SDL_Color S_White  = {0xFF, 0xFF, 0xFF, 0};
static const SDL_Color S_Green  = {0x00, 0xFF, 0x00, 0};
static const SDL_Color S_Cyan   = {0x00, 0xFF, 0xFF, 0};
static const SDL_Color S_Red    = {0xFF, 0x00, 0x00, 0};
static const SDL_Color S_Yellow = {0xFF, 0xFF, 0x00, 0};
static const SDL_Color S_Black  = {0x00, 0x00, 0x00, 0};
static const SDL_Color S_Grey   = {0x88, 0x88, 0x88, 0};

SFont_Font* SFont_InitFont(int which)
{
    int i = 0;
    SFont_Font* Font;
    TTF_Font* tmpfont;
	static int once = 1;
	SDL_Color Color;
	int Size;
	char fontname[MAXPATHLEN];

	/* First time in, initialize the TTF subsysten. */
	if (once) {
		once = 0;

		if (TTF_Init() == -1) {
			fprintf(stderr,"Bad return code from TTF_Init (%s)\n", TTF_GetError());
			return NULL;
		}
		atexit(TTF_Quit);
	}

	Size = 10;

	switch(which) {
	case F_REG_GREEN:
		Color = S_Green;
		break;
	case F_REG_CYAN:
		Color = S_Cyan;
		break;
	case F_REG_YELLOW:
		Color = S_Yellow;
		break;
	case F_REG_RED:
		Color = S_Red;
		break;
	case F_REG_GREY:
		Color = S_Grey;
		break;
	case F_BIG_RED:
		Size = 46;
		Color = S_Red;
		break;
	default:
		fprintf(stderr, "Unknown font requested.\n");
		return NULL;
		break;
	}

	snprintf(fontname, sizeof(fontname), "%s/fonts/%s", DATADIR, "LiberationMono-Bold.ttf");
	tmpfont = TTF_OpenFont(fontname, Size);
	if (tmpfont == NULL) {
		fprintf(stderr,"Bad return code from TTF_OpenFont (%s)\n",TTF_GetError());
		return NULL;
	}

	Font = malloc(sizeof(SFont_Font));
	if (!Font)
		return NULL;

    for (i=32;i<127;i++) {
		char ca[2];
		SDL_Surface *surface;
		ca[0]=i;
		ca[1]=0;

		surface = TTF_RenderText_Blended(tmpfont, ca, Color);
		Font->CharW[i-32] = surface->w;
		Font->CharH = surface->h;
#if SDL_VERSION_ATLEAST(1,3,0)
		Font->CharSurf[i-32] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
#else
		Font->CharSurf[i-32] = surface;
#endif
    }

	TTF_CloseFont(tmpfont);

    return Font;
}

void SFont_Write(const SFont_Font *Font, int x, int y, const char *text)
{
    int charoffset;
    SDL_Rect srcrect, dstrect;
    int i;

    if(text == NULL)
		return;

    srcrect.y = 0;
    dstrect.y = y+WINTOPOV;
    srcrect.h = dstrect.h = Font->CharH;
    for(i=0; text[i] != '\0' && x <= winwidth ; i++) {
		charoffset = text[i] - 32;
		srcrect.w = dstrect.w = Font->CharW[charoffset];
		srcrect.x = 0;
		dstrect.x = x;
#if SDL_VERSION_ATLEAST(1,3,0)
		SDL_RenderCopy(renderer, Font->CharSurf[charoffset], &srcrect, &dstrect);
#else
		SDL_BlitSurface(Font->CharSurf[charoffset], &srcrect, screen, &dstrect);
#endif
		dstrect.h=srcrect.h;              /* changed by blit */
		x += dstrect.w;
    }
}

int SFont_TextWidth(const SFont_Font *Font, const char *text)
{
    int charoffset=0;
    int width = 0;
    int i;

    if (text == NULL)
		return 0;

    for(i=0; text[i] != '\0'; i++) {
		charoffset = text[i] - 32;
		width += Font->CharW[charoffset];
    }

    return width;
}

int SFont_TextHeight(const SFont_Font* Font)
{
    return Font->CharH;
}
void SFont_WriteCenter(const SFont_Font *Font,
					   int y, const char *text)
{
	SFont_Write(Font, (winwidth - SFont_TextWidth(Font, text))/2, y, text);
}

void SFont_WriteRight(const SFont_Font *Font,
					  int y, const char *text)
{
	SFont_Write(Font, winwidth - SFont_TextWidth(Font, text), y, text);
}
#endif
