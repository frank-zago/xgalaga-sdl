/* images.h.  Contains the #defines and extern's for all the image management
   data [BDyess] */
/* automatically created by scripts/mkimgsrc */

struct W_Image {
	int width, height;
	int frames;				/* nviews for ships */
	char *filename;			/* filename without .xpm/.xbm extension*/
	SDL_Surface *surface;	/* ptr to SDL surface */
};

enum {
	I_FIRST,

	I_AL = I_FIRST,
	I_ALIEN1,
	I_ALIEN10,
	I_ALIEN11,
	I_ALIEN12,
	I_ALIEN13,
	I_ALIEN14,
	I_ALIEN15,
	I_ALIEN16,
	I_ALIEN17,
	I_ALIEN2,
	I_ALIEN3,
	I_ALIEN4,
	I_ALIEN5,
	I_ALIEN6,
	I_ALIEN7,
	I_ALIEN8,
	I_ALIEN9,
	I_ETORP,
	I_EXPLOSION,
	I_EXTRA,
	I_MINISHIP,
	I_MTORP,
	I_PAUSE,
	I_PLAYER1,
	I_PLAYER2,
	I_PLAYER3,
	I_PR_BRAIN,
	I_PR_DOUB,
	I_PR_EXTRABULLET,
	I_PR_LEMON,
	I_PR_SHIELD,
	I_PR_SING,
	I_PR_SPEED,
	I_PR_TRIP,
	I_S1000,
	I_S2000,
	I_S4000,
	I_S500,
	I_SHIELD,
	I_TITLE,

	F_REG_GREEN,
	F_REG_CYAN,
	F_REG_YELLOW,
	F_REG_RED,
	F_REG_GREY,
	F_BIG_RED,

	I_LAST = F_BIG_RED,
};
