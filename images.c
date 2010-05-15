//todo licence

#include <SDL/SDL.h>
#include <SDL_image.h>

#include "images.h"
#include "struct.h"
#include "proto.h"

static struct W_Image imagearray[] = {
	/* Bitmaps. */
	{0, 0, 0, "al", NULL },
	{0, 0, 0, "alien1", NULL },
	{0, 0, 0, "alien10", NULL },
	{0, 0, 0, "alien11", NULL },
	{0, 0, 0, "alien12", NULL },
	{0, 0, 0, "alien13", NULL },
	{0, 0, 0, "alien14", NULL },
	{0, 0, 0, "alien15", NULL },
	{0, 0, 0, "alien16", NULL },
	{0, 0, 0, "alien17", NULL },
	{0, 0, 0, "alien2", NULL },
	{0, 0, 0, "alien3", NULL },
	{0, 0, 0, "alien4", NULL },
	{0, 0, 0, "alien5", NULL },
	{0, 0, 0, "alien6", NULL },
	{0, 0, 0, "alien7", NULL },
	{0, 0, 0, "alien8", NULL },
	{0, 0, 0, "alien9", NULL },
	{0, 0, 0, "etorp", NULL },
	{0, 0, 5, "explosion", NULL },
	{0, 0, 0, "extra", NULL },
	{0, 0, 0, "miniship", NULL },
	{0, 0, 0, "mtorp", NULL },
	{0, 0, 0, "pause", NULL },
	{0, 0, 0, "player1", NULL },
	{0, 0, 0, "player2", NULL },
	{0, 0, 0, "player3", NULL },
	{0, 0, 0, "pr_brain", NULL },
	{0, 0, 0, "pr_doub", NULL },
	{0, 0, 0, "pr_extrabullet", NULL },
	{0, 0, 0, "pr_lemon", NULL },
	{0, 0, 0, "pr_shield", NULL },
	{0, 0, 0, "pr_sing", NULL },
	{0, 0, 0, "pr_speed", NULL },
	{0, 0, 0, "pr_trip", NULL },
	{0, 0, 0, "s1000", NULL },
	{0, 0, 0, "s2000", NULL },
	{0, 0, 0, "s4000", NULL },
	{0, 0, 0, "s500", NULL },
	{0, 0, 0, "shield", NULL },
	{0, 0, 0, "title", NULL },

	/* Fonts */
	{0, 0, 1, "sfont_reg_green", NULL },
	{0, 0, 1, "sfont_reg_cyan", NULL },
	{0, 0, 1, "sfont_reg_yellow", NULL },
	{0, 0, 1, "sfont_reg_red", NULL },
	{0, 0, 1, "sfont_reg_grey", NULL },
	{0, 0, 1, "sfont_big_red", NULL },
};

static struct W_Image *loadImage(int offset)
{
	struct W_Image *image = &imagearray[offset];
	SDL_Surface *simage;
	char fname[100];

    if (image->surface)
		return image;

	sprintf(fname, "images/%s.png", image->filename);
	simage = IMG_Load(fname);
	if (!simage)
		return NULL;

#if 1
	image->surface = SDL_DisplayFormat(simage);
	SDL_FreeSurface(simage);
#else
	image->surface = simage;
	simage = NULL;
#endif

	if (!image->surface)
		return NULL;

	/* Set transparent background. */
    SDL_SetColorKey(image->surface, (SDL_SRCCOLORKEY),
					SDL_MapRGB(image->surface->format, 0x00, 0x00, 0x00));

	if (image->frames == 0) {
		/* if an image doesn't have a number of frames, then guess how
		 * many there are by assuming that a frame is square. This is
		 * easily overriden by setting frames = 1. */
		if ((image->surface->h % image->surface->w) == 0) {
			/* even multiple */
			image->frames = image->surface->h / image->surface->w;
		} else {
			image->frames = 1;
		}
    }

    image->height = image->surface->h / image->frames;
    image->width = image->surface->w;

    return image;
}

int loadAllImages()
{
	int i;

	for (i = I_FIRST; i <= I_LAST; i++)
		if (!loadImage(i)) {
			printf("FZ %d\n", i);
			return 0;
		}

	return 1;
}

struct W_Image *getImage(int offset)
{
	return &imagearray[offset];
}
