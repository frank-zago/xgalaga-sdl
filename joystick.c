/*
 * Support for joysticks under Linux
 *
 * Wouter Verhelst <wouter@debian.org>, 2003
 *
 * Distribute under the terms of the GPL.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "data.h"
#include "struct.h"
#include "defs.h"

static SDL_Joystick *js_device;

/*
 * Initialize the joystick device
 */
void init_joystick()
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	if (SDL_NumJoysticks()>0) {
		// Open joystick
		js_device = SDL_JoystickOpen(0);
  
		if (js_device) {
			printf("Opened Joystick 0\n");
			printf("Name: %s\n", SDL_JoystickName(0));
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(js_device));
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(js_device));
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(js_device));

			//update_curstate();

		}
		else
			printf("Couldn't open Joystick 0\n");
	}
}
