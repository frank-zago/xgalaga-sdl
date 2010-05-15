/*
 * Support for joysticks under Linux
 *
 * Wouter Verhelst <wouter@debian.org>, 2003
 *
 * Distribute under the terms of the GPL.
 */
#ifdef __linux__
#include <linux/joystick.h>
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

struct js_state js_curstate;
int js_device;

/* make struct js_state be up-to-date */
void update_curstate() {
  struct js_event event;
  int npval;

  while(read(js_device,&event,sizeof(struct js_event))>0) {
    npval=event.type&JS_EVENT_INIT?0:-1;
    event.type &= ~JS_EVENT_INIT;
    if(event.type==JS_EVENT_BUTTON) {
      js_curstate.but+=event.value?1:npval;
    } else if(event.type==JS_EVENT_AXIS) {
      if(event.number%2==0) {
	js_curstate.dir=event.value;
      }
    }
  }
  if (errno !=EAGAIN) {
    perror("reading joystick device");
    return;
  }
}

/*
 * Initialize the joystick device
 */
void init_joystick() {
  int ioc=0;
  js_device = open("/dev/js0",O_RDONLY|O_NONBLOCK);
  if(js_device<0) {
//    perror("open(/dev/js0)");
    close(js_device);
    js_device=0;
  }
  else if(ioctl(js_device,JSIOCGVERSION,&ioc)==-1) {
    /* Driver too old */
    fprintf(stderr,"Can't read joystick: driver too old\n");
    close(js_device);
    js_device=0;
  }
  else {
    update_curstate();
  }
}

void do_joystick(int* x, int* y, int* but) {
  if (js_device>0 && !mouseControl) {
    update_curstate();
    *but=(js_curstate.but?W_LBUTTON:0);
    if(*but && gameOver) {
      gameOver=0;
    }
    if (js_curstate.dir>0 && !mouseControl)
      *x = WINWIDTH;
    else if(js_curstate.dir<0)
      *x = 0;
    else
      *x=plx;
  }
}
#else
js_device=0;
#endif /* __linux__ */
