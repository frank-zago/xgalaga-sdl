/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
 * Copyright (c) 1994 Sujal M. Patel (smpatel@wam.umd.edu)
 * Copyright (c) 2000 Andy Tarkinson <atark@thepipeline.net>
 * Copyright (c) 2010 Frank Zago
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

#ifdef NOSOUND

void init_sound () {}
void sound_exit(void) {}
void play_sound (int sound) {}

#else
#include <SDL/SDL_mixer.h>

#include "xgalaga.h"

static int audioOK;

static const char *FILENAME[] = {
	"explode.wav",
	"firetorp.wav",
	"shield.wav",
	"torphit.wav",
	"explode_big.wav",
	"ddloo.wav",
	"warp.wav",
	"smart.wav",
};

#define NUM_SOUNDS  (sizeof(FILENAME)/sizeof(char*))

static Mix_Chunk *sounds[NUM_SOUNDS];

void init_sound (void)
{
	unsigned int i;
	char filename[MAXFILENAME];

    /* Open the audio device */
    if (Mix_OpenAudio(11025, AUDIO_U8, 1, 512) < 0 ) {
        fprintf(stderr,
				"Warning: Couldn't set 11025 Hz 8-bit audio\n- Reason: %s\n",
				SDL_GetError());
		return;
    }

	for (i=0; i<NUM_SOUNDS; i++) {
		snprintf(filename, MAXFILENAME, "%s/sounds/%s", DATADIR, FILENAME[i]);
		filename[MAXFILENAME-1] = '\0';
		sounds[i] = Mix_LoadWAV(filename);

		if (!sounds[i])
			fprintf(stderr, "Warning: Couldn't load sound %s\n", filename);
	}

	audioOK = 1;
}

void sound_exit(void)
{
	unsigned int i;

	if (audioOK) {
		Mix_CloseAudio();
		for (i=0;i<NUM_SOUNDS;i++) {
			Mix_FreeChunk(sounds[i]);
		}
		audioOK = 0;
	}
}

void play_sound (int sound)
{
	if (audioOK && playSounds)
		Mix_PlayChannel(sound, sounds[sound], 0);
}

#endif	/* NOSOUND */
