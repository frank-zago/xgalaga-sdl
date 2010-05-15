#ifndef _LINUX_PCSP_H
#define _LINUX_PCSP_H
/*
 * include/linux/pcsp.h
 *
 * /dev/pcsp implementation (dsp & audio for PC Speaker)
 *
 * Copyright (C) 1993, 1994  Michael Beck
 * Some Parts (C) Hannu Savolainen 1992, 1993
 *
 * Minor Alterations for Paradise Sound Client
 */

#include <sys/ioctl.h>

/* MUST BE defined from 0.5 */
/* #define SOUND_VERSION	0x60		*/

#define PCSP_MAJOR		30
#define PCSP_MIXER_MINOR	0
#define PCSP_DSP_MINOR		3
#define PCSP_AUD_MINOR 		4

/* card ID, real soundcards use 0 to ... */

#define SNDCARD_PCSP	128
#define SNDCARD_STO1	129
#define SNDCARD_DACM	130
#define SNDCARD_DACS	131

/* IOCTL commands, compatible to real /dev/dsp */
/* SAMPLESIZE returns only the value, doesn't support changes */

#define SNDCTL_DSP_RESET                _IO  ('P', 0)
#define SNDCTL_DSP_SYNC                 _IO  ('P', 1)
#define SNDCTL_DSP_SPEED                _IOWR('P', 2, int)
#define SNDCTL_DSP_STEREO               _IOWR('P', 3, int)
#define SNDCTL_DSP_GETBLKSIZE           _IOWR('P', 4, int)
/* #define SNDCTL_DSP_SAMPLESIZE           _IOWR('P', 5, int) */

#define SOUND_PCM_WRITE_CHANNELS        _IOWR('P', 6, int)
#define SOUND_PCM_WRITE_FILTER          _IOWR('P', 7, int)
/* #define SOUND_PCM_WRITE_BITS            SNDCTL_DSP_SAMPLESIZE */
#define SOUND_PCM_WRITE_RATE            SNDCTL_DSP_SPEED

#define SOUND_PCM_READ_RATE             _IOR ('P', 2, int)
#define SOUND_PCM_READ_CHANNELS         _IOR ('P', 6, int)
#define SOUND_PCM_READ_BITS             _IOR ('P', 5, int)
#define SOUND_PCM_READ_FILTER           _IOR ('P', 7, int)


/* IOCTL for changing the play-device, real sample rate etc. */

#define PCSP_SET_DEV					0x00014350
#define PCSP_GET_DEV					0x00024350
#define PCSP_SET_PORTS					0x00034350
#define PCSP_GET_PORTS					0x00044350
#define PCSP_SET_VOL					0x00054350
#define PCSP_GET_VOL					0x00064350
#define PCSP_SET_SRATE					0x00074350
#define PCSP_GET_SRATE					0x00084350
#define PCSP_GET_MEASURE				0x00094350

/*********************************************
 * IOCTL commands for /dev/mixer
 */
	
/* Mixer devices */

#define SOUND_MIXER_NRDEVICES	12
#define SOUND_MIXER_VOLUME	0
#define SOUND_MIXER_BASS	1
#define SOUND_MIXER_TREBLE	2
#define SOUND_MIXER_SYNTH	3
#define SOUND_MIXER_PCM		4
#define SOUND_MIXER_SPEAKER	5
#define SOUND_MIXER_LINE	6
#define SOUND_MIXER_MIC		7
#define SOUND_MIXER_CD		8
#define SOUND_MIXER_IMIX	9	/*  Recording monitor  */
#define SOUND_MIXER_ALTPCM	10
#define SOUND_MIXER_RECLEV	11	/* Recording level */

/* Some on/off settings (SOUND_SPECIAL_MIN - SOUND_SPECIAL_MAX) */
/* Not counted to SOUND_MIXER_NRDEVICES, but use the same number space */
#define SOUND_ONOFF_MIN		28
#define SOUND_ONOFF_MAX		30
#define SOUND_MIXER_MUTE	28	/* 0 or 1 */
#define SOUND_MIXER_ENHANCE	29	/* Enhanced stereo (0, 40, 60 or 80) */
#define SOUND_MIXER_LOUD	30	/* 0 or 1 */

/* Note!	Number 31 cannot be used since the sign bit is reserved */

#define SOUND_DEVICE_LABELS	{"Vol  ", "Bass ", "Trebl", "Synth", "Pcm  ", "Spkr ", "Line ", \
				 "Mic  ", "CD   ", "Mix  ", "Pcm2 ", "rec"}

#define SOUND_DEVICE_NAMES	{"vol", "bass", "treble", "synth", "pcm", "speaker", "line", \
				 "mic", "cd", "mix", "pcm2", "rec"}

/*	Device bitmask identifiers	*/

#define SOUND_MIXER_RECSRC	0xff	/* Arg contains a bit for each recording source */
#define SOUND_MIXER_DEVMASK	0xfe	/* Arg contains a bit for each supported device */
#define SOUND_MIXER_RECMASK	0xfd	/* Arg contains a bit for each supported recording source */
#define SOUND_MIXER_CAPS	0xfc
	#define SOUND_CAP_EXCL_INPUT	0x00000001	/* Only one recording source at a time */
#define SOUND_MIXER_STEREODEVS	0xfb	/* Mixer channels supporting stereo */

/*	Device mask bits	*/

#define SOUND_MASK_VOLUME	(1 << SOUND_MIXER_VOLUME)
#define SOUND_MASK_BASS		(1 << SOUND_MIXER_BASS)
#define SOUND_MASK_TREBLE	(1 << SOUND_MIXER_TREBLE)
#define SOUND_MASK_SYNTH	(1 << SOUND_MIXER_SYNTH)
#define SOUND_MASK_PCM		(1 << SOUND_MIXER_PCM)
#define SOUND_MASK_SPEAKER	(1 << SOUND_MIXER_SPEAKER)
#define SOUND_MASK_LINE		(1 << SOUND_MIXER_LINE)
#define SOUND_MASK_MIC		(1 << SOUND_MIXER_MIC)
#define SOUND_MASK_CD		(1 << SOUND_MIXER_CD)
#define SOUND_MASK_IMIX		(1 << SOUND_MIXER_IMIX)
#define SOUND_MASK_ALTPCM	(1 << SOUND_MIXER_ALTPCM)
#define SOUND_MASK_RECLEV	(1 << SOUND_MIXER_RECLEV)

#define SOUND_MASK_MUTE		(1 << SOUND_MIXER_MUTE)
#define SOUND_MASK_ENHANCE	(1 << SOUND_MIXER_ENHANCE)
#define SOUND_MASK_LOUD		(1 << SOUND_MIXER_LOUD)

#define MIXER_READ(dev)		_IOR('M', dev, int)
#define SOUND_MIXER_READ_VOLUME		MIXER_READ(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_READ_BASS		MIXER_READ(SOUND_MIXER_BASS)
#define SOUND_MIXER_READ_TREBLE		MIXER_READ(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_READ_SYNTH		MIXER_READ(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_READ_PCM		MIXER_READ(SOUND_MIXER_PCM)
#define SOUND_MIXER_READ_SPEAKER	MIXER_READ(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_READ_LINE		MIXER_READ(SOUND_MIXER_LINE)
#define SOUND_MIXER_READ_MIC		MIXER_READ(SOUND_MIXER_MIC)
#define SOUND_MIXER_READ_CD		MIXER_READ(SOUND_MIXER_CD)
#define SOUND_MIXER_READ_IMIX		MIXER_READ(SOUND_MIXER_IMIX)
#define SOUND_MIXER_READ_ALTPCM		MIXER_READ(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_READ_RECLEV		MIXER_READ(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_READ_MUTE		MIXER_READ(SOUND_MIXER_MUTE)
#define SOUND_MIXER_READ_ENHANCE	MIXER_READ(SOUND_MIXER_ENHANCE)
#define SOUND_MIXER_READ_LOUD		MIXER_READ(SOUND_MIXER_LOUD)

#define SOUND_MIXER_READ_RECSRC		MIXER_READ(SOUND_MIXER_RECSRC)
#define SOUND_MIXER_READ_DEVMASK	MIXER_READ(SOUND_MIXER_DEVMASK)
#define SOUND_MIXER_READ_RECMASK	MIXER_READ(SOUND_MIXER_RECMASK)
#define SOUND_MIXER_READ_STEREODEVS	MIXER_READ(SOUND_MIXER_STEREODEVS)
#define SOUND_MIXER_READ_CAPS		MIXER_READ(SOUND_MIXER_CAPS)

#define MIXER_WRITE(dev)		_IOWR('M', dev, int)
#define SOUND_MIXER_WRITE_VOLUME	MIXER_WRITE(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_WRITE_BASS		MIXER_WRITE(SOUND_MIXER_BASS)
#define SOUND_MIXER_WRITE_TREBLE	MIXER_WRITE(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_WRITE_SYNTH		MIXER_WRITE(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_WRITE_PCM		MIXER_WRITE(SOUND_MIXER_PCM)
#define SOUND_MIXER_WRITE_SPEAKER	MIXER_WRITE(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_WRITE_LINE		MIXER_WRITE(SOUND_MIXER_LINE)
#define SOUND_MIXER_WRITE_MIC		MIXER_WRITE(SOUND_MIXER_MIC)
#define SOUND_MIXER_WRITE_CD		MIXER_WRITE(SOUND_MIXER_CD)
#define SOUND_MIXER_WRITE_IMIX		MIXER_WRITE(SOUND_MIXER_IMIX)
#define SOUND_MIXER_WRITE_ALTPCM	MIXER_WRITE(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_WRITE_RECLEV	MIXER_WRITE(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_WRITE_MUTE		MIXER_WRITE(SOUND_MIXER_MUTE)
#define SOUND_MIXER_WRITE_ENHANCE	MIXER_WRITE(SOUND_MIXER_ENHANCE)
#define SOUND_MIXER_WRITE_LOUD		MIXER_WRITE(SOUND_MIXER_LOUD)

#define SOUND_MIXER_WRITE_RECSRC	MIXER_WRITE(SOUND_MIXER_RECSRC)

/*
 *	The following mixer ioctl calls are compatible with the BSD driver by
 *	  Steve Haehnichen <shaehnic@ucsd.edu>
 */

typedef unsigned char BYTE;
typedef unsigned char FLAG;
struct stereo_vol
{
  BYTE l;			/* Left volume */
  BYTE r;			/* Right volume */
};

#define MIXER_IOCTL_SET_LEVELS 		_IOW ('s', 20, struct sb_mixer_levels)
#define MIXER_IOCTL_SET_PARAMS 		_IOW ('s', 21, struct sb_mixer_params)
#define MIXER_IOCTL_READ_LEVELS 	_IOR ('s', 22, struct sb_mixer_levels)
#define MIXER_IOCTL_READ_PARAMS 	_IOR ('s', 23, struct sb_mixer_params)
#define MIXER_IOCTL_RESET		_IO  ('s', 24)
 
/*
 * Mixer volume levels for MIXER_IOCTL_SET_VOL & MIXER_IOCTL_READ_VOL
 */
struct sb_mixer_levels
{
  struct stereo_vol master;	/* Master volume */
  struct stereo_vol voc;	/* DSP Voice volume */
  struct stereo_vol fm;		/* FM volume */
  struct stereo_vol line;	/* Line-in volume */
  struct stereo_vol cd;		/* CD audio */
  BYTE mic;			/* Microphone level */
};

/*
 * Mixer parameters for MIXER_IOCTL_SET_PARAMS & MIXER_IOCTL_READ_PARAMS
 */
struct sb_mixer_params
{
  BYTE record_source;		/* Recording source (See SRC_xxx below) */
  FLAG hifreq_filter;		/* Filter frequency (hi/low) */
  FLAG filter_input;		/* ANFI input filter */
  FLAG filter_output;		/* DNFI output filter */
  FLAG dsp_stereo;		/* 1 if DSP is in Stereo mode */
};

#define SRC_MIC         1	/* Select Microphone recording source */
#define SRC_CD          3	/* Select CD recording source */
#define SRC_LINE        7	/* Use Line-in for recording source */

#endif
