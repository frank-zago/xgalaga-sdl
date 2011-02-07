/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/stat.h>
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <ctype.h>
#include <pwd.h>
#include <string.h>

#include "xgalaga.h"

#include <SDL/SDL_endian.h>

#define NUM_MY_SCORES 10

static char new_name[20];
static int nnpos=0;
static int my_thisplace = -1;

static struct high_score {
    char name[20];
    Uint32 score;
    Uint32 level;
} my_scores[NUM_MY_SCORES];

void do_name(void)
{
    char buf[21];
    static int init = 0;

    if(!init) {
		strcpy(new_name, getUsersFullName());
		nnpos = strlen(new_name);
		init = 1;
    }
    SFont_WriteCenter(fnt_reg_red, 250, "Great score! Enter your name:");
    sprintf(buf, "%s_", new_name);
    SFont_WriteCenter(fnt_reg_cyan, 250 + SFont_TextHeight(fnt_reg_cyan), buf);
}

char *getUsersFullName(void)
{
    struct passwd *pass;
    char *comma;
    char *cp1, *cp2;
    static char fullname[80];

    /* Get user information from password file */
    if (!(pass = getpwuid(getuid())))
        return("Anonymous?");       /* Unknown user oops. */

    /* find a comma indicating further info after name */
    comma = strchr(pass->pw_gecos, ',');

    /* NULL out the comma */
    if (comma) *comma = '\0';

    /* Use the nickname if not null otherwise password file name */
    cp1 = pass->pw_gecos;
    cp2 = fullname;

    /* Search through the gecos field looking for an '&' which on very
     * old UNIX systems is supposed to be the users user name with the
     * first letter uppercased.
     */
    while(*cp1)
    {
        /* Look for the '&' symbol */
        if(*cp1 != '&')
            *cp2++ = *cp1++;
        else
        {
            /* A ha. Now copy the users name to be in place of '&' */
            strcpy(cp2, pass->pw_name);

            /* Convert the first letter to uppercase. */
            if(islower(*cp2))
                *cp2 = toupper(*cp2);

            /* Continue with the remaining string */
            while(*cp2) cp2++;
                cp1++;
        }
    }

    /* shorten to 20 chars */
    fullname[19] = 0;

    /* Return their name without any trailing stuff */
    return(fullname);
}

static void save_scores(void)
{
    int i;
    int hsf;
    long x;
    char my_file_name [256], *home;

    if((home = getenv("HOME"))) {
		snprintf(my_file_name, sizeof(my_file_name)-1, "%s/.xgalscores", home);
		hsf = open(my_file_name, O_WRONLY | O_TRUNC | O_CREAT, 0644);

		if(hsf < 0) {
			fprintf(stderr, "Couldn't write scores file %s\n", my_file_name);
			return;
		}
		for(i=0;i<NUM_MY_SCORES;i++) {
			if(write(hsf, my_scores[i].name, 20) < 20)
				goto error2;
			x=SDL_SwapBE32(my_scores[i].score);
			if(write(hsf, &x, sizeof(Uint32)) < (ssize_t)sizeof(Uint32))
				goto error2;
			x=SDL_SwapBE32(my_scores[i].level);
			if(write(hsf, &x, sizeof(Uint32)) < (ssize_t)sizeof(Uint32))
				goto error2;
		}
        close(hsf);
    }

    return;
error2:
    fprintf(stderr, "Error saving high scores file %s\n", my_file_name);
    return;
}

void add_score(char *name, unsigned int score)
{
    int i,j ; /* ,k; */

    load_scores();

    for(i=0;i<NUM_MY_SCORES;i++) {
		if(score > my_scores[i].score) {
			for(j=NUM_MY_SCORES-1;j>i;j--) {
				strcpy(my_scores[j].name, my_scores[j-1].name);
				my_scores[j].score = my_scores[j-1].score;
				my_scores[j].level = my_scores[j-1].level;
			}
			strcpy(my_scores[i].name, name);
			my_scores[i].score = score;
			my_scores[i].level = level;
			my_thisplace = i;
			break;
		}
    }
    save_scores();
}

/* Read a new name for highscore. */
int score_key(SDLKey key)
{
	switch(key) {
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
	case SDLK_ESCAPE:
		gstate = INTRO;
		add_score(new_name, score);
		title_page = 1;
		pagetimer = 300;
		break;

	case SDLK_BACKSPACE:
	case SDLK_DELETE:
	case SDLK_KP4:
	case SDLK_LEFT:
		if (nnpos > 0) {
			nnpos--;
			new_name[nnpos] = 0;
		}
		break;

	case SDLK_HOME:
		nnpos = 0;
		new_name[nnpos] = 0;
		break;

	default:
		if (nnpos < 19 &&
			key >= SDLK_SPACE &&
			key <= SDLK_z) {
			new_name[nnpos++] = key;
			new_name[nnpos] = 0;
		}
		break;
	}

	return 1;
}


int check_score(unsigned int score)
{
    int i;

    load_scores(); /* in case someone else has gotten a high score */

    for(i=0;i<NUM_MY_SCORES;i++) {
		if(score > my_scores[i].score)
			return 1;
    }

    my_thisplace = -1;
    return 0;
}


void show_scores(int top)
{
    int i;
    char buf[60];
	int dy = SFont_TextHeight(fnt_reg_yellow);
	const char *labels = "Rank  Name                      Score   Level";
	int length;

    SFont_WriteCenter(fnt_reg_yellow, top, "High scores");
	top += dy;
    SFont_WriteCenter(fnt_reg_yellow, top, labels);

	/* Draw a line. */
	length = SFont_TextWidth(fnt_reg_yellow, labels);
	S_DrawRect((winwidth-length)/2, top + 1 + SFont_TextHeight(fnt_reg_yellow),
			   length, 1,
			   SDL_MapRGB(screen->format, 0xff, 0, 0));

    for(i=0;i<NUM_MY_SCORES;i++) {
		SFont_Font *font = i==my_thisplace ? fnt_reg_red : fnt_reg_grey;

		sprintf(buf, "  %2d. %-20s     %7lu %5lu",
				i+1, my_scores[i].name, 
				(unsigned long)my_scores[i].score,
				(unsigned long)my_scores[i].level);
		SFont_WriteCenter(font, top+(3+i)*dy, buf);
    }
}

void load_scores(void)
{
    int i;
    int hsf;
    char my_file_name[256], *home;

    if((home = getenv("HOME"))) {
		snprintf(my_file_name, sizeof(my_file_name)-1, "%s/.xgalscores", home);
		hsf = open(my_file_name, O_RDONLY);
		if(hsf <0 ) {
			fprintf(stderr, "Trouble opening high scores file '%s'\n", my_file_name);
			for(i=0;i<NUM_MY_SCORES;i++) {
				my_scores[i].name[0]=0;
				my_scores[i].score = 0;
				my_scores[i].level = 0;
			}
		} else {
			for(i=0;i<NUM_MY_SCORES;i++) {
				if(read(hsf, my_scores[i].name, 20) < 20)
					goto error2;
				if(read(hsf, &my_scores[i].score, sizeof(Uint32)) < (ssize_t)sizeof(Uint32))
					goto error2;
				if(read(hsf, &my_scores[i].level, sizeof(Uint32)) < (ssize_t)sizeof(Uint32))
					goto error2;
				my_scores[i].score = SDL_SwapBE32(my_scores[i].score);
				my_scores[i].level = SDL_SwapBE32(my_scores[i].level);
			}
		}
		close(hsf);
    } else {
		fprintf(stderr, "No HOME variable, so no personal score file.\n");
		for(i=0;i<NUM_MY_SCORES;i++) {
			my_scores[i].name[0]=0;
			my_scores[i].score = 0;
			my_scores[i].level = 0;
		}
    }
    return;

error2:
    if(i>0)
		fprintf(stderr, "Error reading high scores file '%s'\n", my_file_name);
    for(i=0;i<NUM_MY_SCORES;i++) {
		my_scores[i].name[0]=0;
		my_scores[i].score = 0;
		my_scores[i].level = 0;
    }
    close(hsf);
}

void print_scores(void)
{
    int i;

    load_scores();

    printf("-----------------------------------------------\n");
    printf("\nYour High Scores:\n");
    printf("--------------------------------------\n");
    printf("%-20s %8s %8s\n", "name", "score", "level");
    printf("--------------------------------------\n");
    for(i=0;i<NUM_MY_SCORES;i++) {
	if(my_scores[i].score == 0)
	    break;
	printf("%-20s %8lu %8lu\n", my_scores[i].name,
	       (unsigned long)my_scores[i].score, (unsigned long)my_scores[i].level);
    }
    printf("--------------------------------------\n");
}
