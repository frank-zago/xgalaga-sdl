bin_PROGRAMS = xgalaga-sdl

xgalaga_sdl_LDADD = @SDL_LIBS@ @SDL_ttf_LIBS@ @SDL2_LIBS@ @SDL2_ttf_LIBS@
xgalaga_sdl_CFLAGS = -include config.h -DDATADIR=\"@datadir@\" \
	@SDL_CFLAGS@ @SDL_ttf_CFLAGS@ @SDL2_CFLAGS@ @SDL2_ttf_CFLAGS@

xgalaga_sdl_SOURCES = \
	SFont.c \
	SFont.h \
	TFont.c \
	data.c \
	data.h \
	defs.h \
	explosions.c \
	frate.c \
	highscore.c \
	images.c \
	images.h \
	joystick.c \
	main.c \
	pathfile.c \
	paths.c \
	paths.h \
	prize.c \
	proto.h \
	score.c \
	sdl.c \
	sound.c \
	sound.h \
	struct.h \
	title.c \
	xgalaga.h
