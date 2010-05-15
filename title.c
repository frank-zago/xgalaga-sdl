/* $Id: title.c,v 1.4 1998/05/11 06:52:59 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include "math.h"
#include "Wlib.h"
#include "defs.h"
#include "images.h"
#include "struct.h"
#include "data.h"
#include "proto.h"

/* for scrolling credits */
const LINESIZE= 70;
const VISIBLELINES=7;
const SPEEDFACTOR=5;
char *gchLBuf;
int giCreditLines=0, giActCreditLine=0;

W_Image *pauseImage, *titleImage;
int pausex, pausey, pauseyspeed=1;

void undo_pause()
{
    W_CacheClearArea(baseWin, pausex-(pauseImage->width/2), pausey-(pauseImage->height/2), 
		     pauseImage->width, pauseImage->height);
}

void do_pause()
{
    if((pausey < pauseImage->height/2) || ((pausey+(pauseImage->height/2)) >= WINHEIGHT))
	pauseyspeed = -pauseyspeed;
    pausey+=pauseyspeed;

    W_DrawImage(baseWin, pausex-(pauseImage->width/2), pausey-(pauseImage->height/2), 0, pauseImage, W_White);
}

void 
center_text(char *text, int y, W_Color color)
{
/*
    W_MaskText(baseWin, WINWIDTH/2 - ((strlen(text)*W_Textwidth)/2)-1, y+1, W_Black,
	       text, strlen(text), W_RegularFont);
*/
    W_MaskText(baseWin, WINWIDTH/2 - ((strlen(text)*W_Textwidth)/2), y, color,
	       text, strlen(text), W_RegularFont);
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
	W_DrawImage(baseWin, WINWIDTH/2 - 30, 220+i*21,
		    0, getImage(a_images[5-i]), W_Red);
	if(i < 5)
	    sprintf(buf, "- %d", (i+1)*100);
	else
	    sprintf(buf, "- ???");
	W_MaskText(baseWin, WINWIDTH/2, 225+i*21,
		   W_Yellow, buf, strlen(buf), W_RegularFont);
    }
    ty = 220+i*21;
    center_text("Ships in convoy are worth 50 points", ty, W_Green); ty += W_Textheight;
    center_text("Bonus ships at 20,000, 50,000, then every 50,000.", ty, W_Green);
}
#endif

static void show_help()
{
    int top = 270;

    center_text("         Keyboard controls           ", top, W_Red);
    center_text("  left/right arrow   move            ", top+10, W_Yellow);
    center_text("  space bar          fire            ", top+20, W_Yellow);
    center_text("  p                  pause           ", top+30, W_Cyan);
    center_text("  q                  end game        ", top+40, W_Cyan);
    center_text("  Q                  quick quit      ", top+50, W_Cyan);
    center_text("  k                  keyboard control", top+60, W_Cyan);
    center_text("  m                  mouse control   ", top+70, W_Cyan);
#ifndef ORIGINAL_XGALAGA
    center_text("  x                  toggle shields  ", top+80, W_Cyan);
#endif
#ifdef SOUND
    center_text("  s                  toggle sound    ", top+90, W_Cyan);
#endif
	  
    center_text("Bonus ships at 20,000, 50,000, then every 50,000.", top+120, W_Green);
    center_text("XGalaga Home page: http://rumsey.org/xgal.html",
		top+150, W_Cyan);
}

int read_credits()
{
    int hsf;
    int i,j,l,lines;
    int nSize;
    char *chFBuf, chLBuf[LINESIZE+1];

    j = 0; lines = 0;
    chFBuf = (char *) malloc (8000+1);
    if (chFBuf == 0) return (-1);
    gchLBuf = (char *) malloc (LINESIZE+1);
    if (gchLBuf == 0) return (-2);
    hsf = open (CREDITSFILE, O_RDONLY);
    if ( hsf > 0 ) 
    {
      l = read (hsf, chFBuf, 8000);
      chFBuf[l] = 0x00;
      if ( l > 0 )
      {
        for (i=0; i<l; i++)
        {
          switch (chFBuf[i])
          {
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
              chLBuf[j] = 0x00;
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
    }
    else
    {
//      printf (chLBuf, sizeof(chLBuf)-1, "Sorry! Could not read CREDITSFILE");
    }
    close(hsf);
    free (chFBuf);
    giCreditLines = lines-1;
  }


void undo_credits()
{
  int hsf;
  int i,j,l,lines;
  int top = 460;
  char chLBuf[LINESIZE+1];

  if ( gchLBuf > 0 )
  {
    j = 0; lines = 0;
    for ( i=giActCreditLine; i<giCreditLines; i++ )
    {
      strncpy (chLBuf, gchLBuf+i*(LINESIZE+1), LINESIZE);
      center_text (chLBuf, top+lines*W_Textheight + ((pagetimer-1) % (W_Textheight*SPEEDFACTOR))/SPEEDFACTOR, W_Black);
      lines++;
      if ( lines > VISIBLELINES ) break;
    } // end for
  }
  else
  {
    snprintf (chLBuf, sizeof(chLBuf)-1, "Sorry no CREDITS data");
    center_text (chLBuf, top+10, W_Black);
  }
}


void show_credits()
{
  int hsf;
  int i,j,l,lines;
  int top = 460;
  char chLBuf[LINESIZE+1];

  if ( gchLBuf > 0 )
  {
    j = 0; lines = 0;
    for ( i=giActCreditLine; i<giCreditLines; i++ )
    {
      strncpy (chLBuf, gchLBuf+i*(LINESIZE+1), LINESIZE);
      j = top + lines*W_Textheight + ((pagetimer-1) % (W_Textheight*SPEEDFACTOR))/SPEEDFACTOR ; 
      center_text (chLBuf,j, W_Green);
      lines++;
      if ( lines > VISIBLELINES ) break;
    } // end for
    W_ClearArea(baseWin,
                0, top,
                WINWIDTH, W_Textheight);
    W_ClearArea(baseWin,
                0, top+(VISIBLELINES+1)*W_Textheight,
                WINWIDTH, W_Textheight);
   }
  else
  {
    snprintf (chLBuf, sizeof(chLBuf)-1, "Sorry no CREDITS data");
    center_text (chLBuf, top+10, W_Green);
  }
}


void do_title()
{
    int ty;
    char vbuf[10];

    W_DrawImage (baseWin, WINWIDTH/2 - titleImage->width/2, 50, 0, titleImage, W_Red);
    sprintf (vbuf, "v%s", VERSION);
    center_text (vbuf, 40+titleImage->height, W_Yellow);

    ty = 60 + titleImage->height;
    if (getting_name)
	title_page = 0;

    switch (title_page) 
    {
    case 0:
	center_text("Copyright (c) 1995-1998   Joe Rumsey", ty, W_Green); ty+= W_Textheight;
	center_text("Maintenance 2008 by Hermann Riedel", ty, W_Green); ty+= 2*W_Textheight;
#ifndef ORIGINAL_XGALAGA
	center_text("XGalaga: Hyperspace 0.9", ty, W_Green); ty+= W_Textheight;
#endif
        show_help();
//        show_credits();
	break;
    case 1:
    default:
	show_scores();
//        show_credits();
	break;
    }

#ifdef __linux__
    if (js_device)
	    center_text("Press k for keyboard or joystick control, m for mouse", WINHEIGHT - 2*W_Textheight, W_Yellow);
    else /* no joystick, so use message below */
#endif
    center_text("Press k for keyboard control, m for mouse", WINHEIGHT - 2*W_Textheight, W_Yellow);
    center_text("Or q to quit", WINHEIGHT - W_Textheight, W_Yellow);

    undo_credits();     // erase with painting black
    pagetimer--;
//    if (!(pagetimer % 50))
    if (!(pagetimer % (W_Textheight*SPEEDFACTOR)))
    {
//printf("%d %d %25s\n", pagetimer, (pagetimer % (W_Textheight*SPEEDFACTOR)),  gchLBuf+giActCreditLine*(LINESIZE+1));
      giActCreditLine++;
      if (giActCreditLine > giCreditLines)
        giActCreditLine = 0; 
    } 
    if (!pagetimer) 
    {
	W_ClearWindow(baseWin);
	title_page++;
	if (title_page > 1)
	  title_page = 0;
	pagetimer = 299;
    }
    show_credits();     // write text
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

