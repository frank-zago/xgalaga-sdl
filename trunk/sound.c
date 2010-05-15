/*
Copyright (c) 2000 Andy Tarkinson <atark@thepipeline.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <config.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include "data.h"
#include "proto.h"

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

void init_sound ()
{
	int i;
	char s[1024];

    /* Open the audio device */
    if (Mix_OpenAudio(11025, AUDIO_U8, 1, 512) < 0 ) {
        fprintf(stderr,
				"Warning: Couldn't set 11025 Hz 8-bit audio\n- Reason: %s\n",
				SDL_GetError());
		return;
    }

	for (i=0;i<NUM_SOUNDS;i++) {

#if 0
		todo;
		strcpy(s, SOUNDDIR);
		strcat(s, "/");
		strcat(s, FILENAME[i]);
#else
		sprintf(s, "sounds/%s", FILENAME[i]);
#endif
		sounds[i] = Mix_LoadWAV(s);

		if (!sounds[i])
			fprintf(stderr,
					"Warning: Couldn't load sound %s\n", s);
	}

	audioOK = 1;
}

void sound_exit(void)
{
	int i;

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
