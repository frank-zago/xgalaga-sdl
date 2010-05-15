/*
 * xgal.sndsrv.c - VoxWare(tm) Compatible Sound - Apr. 1995
 *                 PC Speaker  Compatible Sound 
 *                 This server is FreeBSD Specific.
 *
 * Copyright 1994-1995 Sujal M. Patel (smpatel@wam.umd.edu)
 * Conditions in "copyright.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <machine/soundcard.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>



char *FILENAME[] = {
                     "/explode.raw",
                     "/firetorp.raw",
                     "/shield.raw",
                     "/torphit.raw",
                     "/explode_big.raw",
		     "/ddloo.raw",
                   };

#define NUM_SOUNDS	(sizeof(FILENAME)/sizeof(char*))

signed char *sound_buffer[NUM_SOUNDS];
int sound_size[NUM_SOUNDS];
int fragsize;


/* Terminate: Signal Handler */
void quit ()
{
  exit (0);
}



void init (int argc, char **argv)
{
  int i;
  char s[1024];

  if (argc != 3)
  {
    printf ("This program is only executed by xgal\n");
    exit (1);
  }

  for (i=0; i < NUM_SOUNDS; i++)
  {
    s[0] = 0;
    strcat (s, argv[1]);
    if (s[(int)strlen(s) - 1] == '/') FILENAME[i]++;
    strcat (s, FILENAME[i]);
    FILENAME[i] = malloc ((int)strlen (s));
    strcpy (FILENAME[i],s);
    sound_buffer[i]=NULL;
    sound_size[i]=0;
  }

  signal(SIGTERM, quit);   /* Setup Terminate Signal Handler */
}


/*
   Setup DSP: Opens /dev/dsp or /dev/pcdsp
              Sets fragment size on VoxWare
              Sets speed to 8000hz
              Should set mono mode
              Error checking                
*/
int setup_dsp (char *dspdev,int *is_pcsp)
{
  int dsp, frag, value;
  int mixer;

  dsp = open(dspdev, O_RDWR);
  if (dsp < 1)
  {
    fprintf (stderr, "xgal.sndsrv: Couldn't open DSP %s\n",dspdev);
    return -1;
  }
 
  *is_pcsp = 0;
  fragsize = 0;

  frag = 0x00020009;   /* try 512 bytes, for 1/16 second frag size */
  ioctl(dsp, SNDCTL_DSP_SETFRAGMENT, &frag);
  value = 8010;
  if (ioctl(dsp, SNDCTL_DSP_SPEED, &value))
  {
      fprintf (stderr, "xgal.sndsrv: Couldn't set DSP rate!\n");
  };
  value = 0;
  ioctl(dsp, SNDCTL_DSP_STEREO, &value);
  ioctl(dsp, SNDCTL_DSP_GETBLKSIZE, &fragsize);
  fprintf(stderr,"xgal.sndsrv: fragment set to %d\n",fragsize);

  if (!fragsize)
  { 
    /* Don't Assume just because you can't set the fragment, use proper IOCTL */
    fprintf (stderr, "xgal.sndsrv: Couldn't set Fragment Size.\nAssuming PC Speaker!\n");
    fragsize = 128;
    *is_pcsp = 1;
  } else {
      mixer = open("/dev/mixer",O_RDWR | O_NONBLOCK);
      if(mixer==-1)  {
          fprintf(stderr,"xgal.sndsrv: Couldn't open mixer %s\n","/dev/mixer");
          return(-1);
      };
#if 0
      value=0x6464;
      ioctl(mixer,SOUND_MIXER_WRITE_PCM,&value);
      ioctl(mixer,SOUND_MIXER_WRITE_VOLUME,&value);  /*what does this do?*/
#endif
      close(mixer);

  }
  
  return dsp;
}

/*
   This just keeps the pipe from breaking...
   Eventually I'll look at the xgal signal handlers and
   just trap this.
*/
int do_nothing(void)
{
    fprintf(stderr,"xgal.sndsrv: doing nothing, something is broken\n");
    while(1)  sleep (5);
}

int read_sound(int k)
{
  int i,fd,size;

  /*fprintf(stderr,"loading sound %d, %s\n",k,FILENAME[k]);*/

  fd = open(FILENAME[k], O_RDONLY);
  if(fd<=0) 
  {
    fprintf (stderr, "xgal.sndsrv: The sound %s could not be opened\n", FILENAME[k]);
    sound_size[k]=-1;
    return(0);
  };
  size=lseek(fd,0,SEEK_END);
  sound_size[k]=(size/fragsize)+1;   /*size in fragments*/
  sound_buffer[k]=malloc(sound_size[k]*fragsize);
  if(sound_buffer[k]==NULL)
  {
    fprintf(stderr,"xgal.sndsrv: couldn't malloc memory for sound\n");
    sound_size[k]=-1;
    close(fd);
    return(0);
  };
  lseek(fd,0,SEEK_SET);
  read(fd,sound_buffer[k],size);
  close(fd);
  for(i=0;i<size;i++)  sound_buffer[k][i]^=0x80;
  memset(sound_buffer[k]+size,0,sound_size[k]*fragsize-size);
  
  /*fprintf(stderr,"sound has been loaded, %d bytes\n",size);*/ /*DEBUG*/
  return(1);
}


void do_everything (int dsp, int is_pcsp)
{
  char k;
  int i, j ;
  int terminate = -1;             /* Which Sound to Terminate                              */
  int playing[16];                /* Sound numbers that we are playing                     */
  int position[16];		  /* Current position in each sound file */
  int playnum = 0;                /* Number of sounds currently being played               */
  unsigned char final[512];       /* Final Mixing Buffer                                   */
  int premix[512];
  char *sample;

  for(;;)  {
    terminate = -1;
    /* Try to open a new sound if we get an integer on the 'in' pipe */
    i=read(STDIN_FILENO,&k,sizeof(k));
    if(i==0)  {   /* EOF on pipe means parent has closed its end */
        /*fprintf(stderr,"xgal.sndsrv: shutting down\n"); */
        kill(getpid(), SIGTERM);
    };
    if(i!=-1)  {  /* there was something in the pipe */
        /*fprintf(stderr,"Just read a %d from pipe\n",(int)k);*/ /*DEBUG*/
        /* Negative means terminate the FIRST sound in the buffer */
        if(k<0)  {
            /*fprintf(stderr,"terminating sound\n");*/ /*DEBUG*/
            terminate = 0;
        } else {
            if(sound_size[k]==0) read_sound(k);
            if(sound_size[k]>0 && playnum<16)  {
	        position[playnum]=0;
                playing[playnum++]=k;
                /*fprintf(stderr,"sound %d added to play queue\n",playnum-1);*/ /*DEBUG*/
            };
        };
    };

    /* terminate a sound if necessary */
    for(i=0;i<playnum;i++)
    {
      if((position[i]==sound_size[playing[i]]) || (terminate==i))
      {
        /*fprintf(stderr,"finished playing sound %d\n",i);*/ /*DEBUG*/
	/*fprintf(stderr,"is was at position %d\n",position[i]);*/ /*DEBUG*/
        memmove(playing+i,playing+i+1,(playnum-i)*sizeof(int));
        memmove(position+i,position+i+1,(playnum-i)*sizeof(int));
        playnum--;i--;
      };
    };

    if(playnum)  {
        /* Mix each sound into the final buffer */
        memset(premix,0,sizeof(premix));
        for(i=0;i<playnum;i++)  {
            sample=sound_buffer[playing[i]]+position[i]*fragsize;
            for(j=0;j<fragsize;j++)  {
                premix[j]+=*(sample+j);
            };
            position[i]++;
        };
        for(i=0;i<fragsize;i++)
            final[i]=(premix[i]>255)?255:(premix[i]<-256?0:(premix[i]>>1)+128);
    } else {
        /* 
           We have no sounds to play
           Just fill the buffer with silence and maybe play it 
        */
        memset(final,128,sizeof(final));
    };
    write (dsp, final, fragsize);
    /*
       The sound server is in a tight loop, EXCEPT for this
       write which blocks.  Any optimizations in the above
       code would really be helpful.  Right now the server
       takes up to 7% cpu on a 486DX/50.
    */
  }
}



void main (argc, argv)
int argc;
char **argv;
{
  int dsp, is_pcsp, ppid;
  char filename[512];

  fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);
  init (argc, argv);
  dsp = setup_dsp (argv[2],&is_pcsp);

  if (!dsp) do_nothing();

  do_everything (dsp, is_pcsp);
}
