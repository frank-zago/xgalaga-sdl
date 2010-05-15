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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL/SDL.h>

#include "defs.h"
#include "images.h"
#include "struct.h"
#include "data.h"
#include "proto.h"

/* for scrolling credits */
static const unsigned int LINESIZE= 70;
static const unsigned int VISIBLELINES=7;
static const unsigned int SPEEDFACTOR=3; /* credit speed factor */
static char *gchLBuf;
static int giCreditLines=-1, giActCreditLine=0;

static const unsigned int NUM_TITLE_PAGES = 3;

static struct W_Image *pauseImage, *titleImage;
static int pausex, pausey, pauseyspeed=1;

void do_pause()
{
    if((pausey < pauseImage->height/2) || ((pausey+(pauseImage->height/2)) >= WINHEIGHT))
	pauseyspeed = -pauseyspeed;
    pausey+=pauseyspeed;

    S_DrawImage(pausex-(pauseImage->width/2), pausey-(pauseImage->height/2), 0, pauseImage);
}

#if 0 /* ships vary from level to level now */
static void show_points()
{
    int i, ty;
    int a_images[6] = {
		I_ALIEN1,
		I_ALIEN2,
		I_ALIEN3,
		I_ALIEN4,
		I_ALIEN5,
		I_ALIEN6
    };

    char buf[40];

    for (i=0;i<6;i++) {
		S_DrawImage(WINWIDTH/2 - 30, 220+i*21,
					0, getImage(a_images[5-i]));
		if(i < 5)
			sprintf(buf, "- %d", (i+1)*100);
		else
			sprintf(buf, "- ???");
		W_MaskText(screen, WINWIDTH/2, 225+i*21,
				   S_Yellow, buf, strlen(buf), W_RegularFont);
    }
    ty = 220+i*21;
    center_text("Ships in convoy are worth 50 points", ty, S_Green); ty += RegularFont.height;
    center_text("Bonus ships at 20,000, 50,000, then every 50,000.", ty, S_Green);
}
#endif

static void show_bonuses(int top)
{

}

static void show_help(int top)
{
	int dy = SFont_TextHeight(fnt_reg_red);

    center_text("         Keyboard controls           ", top, fnt_reg_red);
    center_text("  left/right arrow   move            ", top+dy, fnt_reg_yellow);
    center_text("  space bar          fire            ", top+2*dy, fnt_reg_yellow);
    center_text("  p                  pause           ", top+3*dy, fnt_reg_cyan);
    center_text("  q                  end game        ", top+4*dy, fnt_reg_cyan);
    center_text("  Q                  quick quit      ", top+5*dy, fnt_reg_cyan);
    center_text("  k                  start game      ", top+6*dy, fnt_reg_cyan);
    center_text("  s                  toggle sound    ", top+7*dy, fnt_reg_cyan);
#ifndef ORIGINAL_XGALAGA
    center_text("  x                  toggle shields  ", top+8*dy, fnt_reg_cyan);
#endif

    center_text("Bonus ships at 20,000, 50,000, then every 50,000.", top+10*dy, fnt_reg_green);
    center_text("XGalaga Home page: http://rumsey.org/xgal.html", top+12*dy, fnt_reg_cyan);
}
#include <errno.h>
static void read_credits()
{
    int hsf;
    int i,j,l,lines;
    int nSize;
    char *chFBuf, chLBuf[LINESIZE+1];

    j = 0;
	lines = 0;
    chFBuf = malloc (8000+1);	/* fz-hardcoded */
    if (chFBuf == 0)
		return;

    gchLBuf = malloc (LINESIZE+1);
    if (gchLBuf == 0)
		return;

    hsf = open (CREDITSFILE, O_RDONLY);
    if (hsf > 0) {
		l = read (hsf, chFBuf, 8000);
		chFBuf[l] = 0x00;
		if ( l > 0 ) {
			for (i=0; i<l; i++)	{
				switch (chFBuf[i]) {
				case 10 :     // Linefeed
				case 0 :
					chLBuf[j] = 0x00;
					j = 0;
					strncpy (gchLBuf+(lines*(LINESIZE+1)), chLBuf, LINESIZE);
					lines++;
					nSize = (lines+1) * (LINESIZE+1);
					gchLBuf = (char *) realloc (gchLBuf, nSize);        // get space for next line
					break;
				case 1: case 2: case 3: case 4:case 5:
				case 6: case 7: case 8: case 9:
				case 11: case 12: case 13: case 14:case 15:
				case 16: case 17: case 18: case 19:
					// ignore control characters
					break;
				default:
					chLBuf[j++] = chFBuf[i];            // visible character
					if (j>LINESIZE)
					{
						// line too long -> cut without grace
						chLBuf[LINESIZE-1] = 0x00;
						j = 0;
						strncpy (gchLBuf+(lines*(LINESIZE+1)), chLBuf, LINESIZE);
						lines++;
						nSize = (lines+1) * (LINESIZE+1);
						gchLBuf = (char *) realloc (gchLBuf, nSize);        // get space for next line
					}
					break;
				} // end switch
			} // end for
		}

		close(hsf);
    }
    else
    {
//      printf (chLBuf, sizeof(chLBuf)-1, "Sorry! Could not read CREDITSFILE");
    }
    free (chFBuf);
    giCreditLines = lines-1;
}


void show_credits()
{
	int i,j,lines;
	int top = 460;
	char chLBuf[LINESIZE+1];
	int font_height = SFont_TextHeight(fnt_reg_red);

	if ( gchLBuf > 0 ) {
		j = 0; lines = 0;
		for ( i=giActCreditLine; i<giCreditLines; i++ )
		{
			strncpy (chLBuf, gchLBuf+i*(LINESIZE+1), LINESIZE);
			j = top + lines*font_height + ((pagetimer-1) % (font_height*SPEEDFACTOR))/SPEEDFACTOR;
			SFont_WriteCenter(fnt_reg_green, j, chLBuf);
			lines++;
			if ( lines > VISIBLELINES ) break;
		} // end for
	}
	else
	{
		snprintf (chLBuf, sizeof(chLBuf)-1, "Sorry no CREDITS data");
		center_text (chLBuf, top+10, fnt_reg_green);
	}
}


void do_title()
{
    int top = 130;
    char vbuf[10];

    S_DrawImage(WINWIDTH/2 - titleImage->width/2, top, 0, titleImage);
    sprintf (vbuf, "v%s", VERSION);
    center_text (vbuf, top + titleImage->height - 10, fnt_reg_yellow);

    top += titleImage->height + 15;
    if (gstate == GETTING_NAME)
		title_page = 0;

	center_text("Copyright (c) 1995-1998   Joe Rumsey", top, fnt_reg_green);
	top+= SFont_TextHeight(fnt_reg_green);
	center_text("Maintenance 2008 by Hermann Riedel", top, fnt_reg_green);
	top+= 2*SFont_TextHeight(fnt_reg_green);
#ifndef ORIGINAL_XGALAGA
	center_text("XGalaga: Hyperspace 0.9", top, fnt_reg_green);
	top+= SFont_TextHeight(fnt_reg_green);
#endif

	if (title_page == 0)
        show_help(top);
	else if (title_page == 1)
		show_scores(top);
	else if (title_page == 2)
		show_bonuses(top);

	center_text("Press k to start", WINHEIGHT - 2*SFont_TextHeight(fnt_reg_yellow), fnt_reg_yellow);
    center_text("Or q to quit", WINHEIGHT - SFont_TextHeight(fnt_reg_yellow), fnt_reg_yellow);

    pagetimer--;

    if (!(pagetimer % (SFont_TextHeight(fnt_reg_yellow)*SPEEDFACTOR))) {
		giActCreditLine++;
		if (giActCreditLine > giCreditLines)
			giActCreditLine = 0;
    }

    if (!pagetimer) {
		title_page++;
		if (title_page == NUM_TITLE_PAGES)
			title_page = 0;
		pagetimer = 299;
    }

    show_credits();
}

void init_titles()
{
    titleImage = getImage(I_TITLE);
    pauseImage = getImage(I_PAUSE);
    pausex = WINWIDTH/2;
    pausey = WINHEIGHT/2;
    pauseyspeed = 3;
    read_credits();
}

