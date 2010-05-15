/*
 * sound.c - Platform Independant Sound Support - Apr. 1995
 *
 * Copyright 1994-1995 Sujal M. Patel (smpatel@wam.umd.edu)
 * Conditions in "copyright.h"          
 */

#include <config.h>

#ifdef SOUND

#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include "struct.h"
#include "data.h"

static int soundfd;
static char audioOK = 1;
static char sound_flags[20]; /* Sound Flag for sound 1-19 */


void init_sound ()
{
  int i, child,fd[2];
  char *argv[4];
  char filename[512];

  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  /* Do not initialize sound if it is not going to be used -- JEH */
  if (! playSounds)
	  return;

  if(unixSoundPath[0] == '?')  {
      audioOK = 0;
      return;
  };

  /*  Create a pipe, set the write end to close when we exec the sound server,
      and set both (is the write end necessary?) ends to non-blocking   */
  if (pipe(fd)) {
      audioOK = 0;
      return;
  }
  soundfd=fd[1];

  if( !(child=fork()) )  {
      close(fd[1]);
      dup2(fd[0],STDIN_FILENO);
      close(fd[0]);
      sprintf (filename, SOUNDSERVER);
      argv[0] = filename;
      argv[1] = unixSoundPath;
      argv[2] = unixSoundDev;
      argv[3] = NULL;
      execvp(filename, argv);
      fprintf (stderr, "Couldn't Execute Sound Server %s!\n", filename);
      exit (0);
  };
  close(fd[0]);

  sleep(1);

  if (kill(child, 0))  {
      audioOK = 0;  
      close(soundfd);
  };

  for (i = 0; i < 19; i++) sound_flags[i] = 0;
} 

void play_sound (k)
int k;
{
  char c;

  c = k;
  if ((playSounds) && (audioOK))
    if(write (soundfd, &c, sizeof (c)) != sizeof (c))
      audioOK = 0;
}



void maybe_play_sound (k)
int k;
{
  char c;

  if (sound_flags[k] & 1) return;

  sound_flags[k] |= 1;

  c = (unsigned char)(k);
  if ((playSounds) && (audioOK))
    if(write (soundfd, &c, sizeof (c)) != sizeof (c))
      audioOK = 0;
}



void sound_completed (k)
int k;
{
  sound_flags[k] &= ~1;
}



void kill_sound ()
{ 
  char c;

  c = -1;               
  if ((playSounds) && (audioOK))
    if(write (soundfd, &c, sizeof (c)) != sizeof (c))
      audioOK = 0;
}

#endif /* SOUND */
