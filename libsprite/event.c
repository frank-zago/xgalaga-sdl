#include "allincludes.h"

static struct window *
findWindow(window)
    Window  window;
{
    struct windowlist *entry;

    entry = hashtable[hash(window)];
    while (entry != NULL) {
	if (entry->window->window == window)
	    return (entry->window);
	entry = entry->next;
    }
    return (NULL);
}

void
W_GetEvent(wevent)
    W_Event *wevent;
{
/* blocks until an event is received [BDyess] */
    XEvent  event;

    if (W_isEvent) {
	*wevent = W_myevent;
	W_isEvent = 0;
	return;
    }
    XNextEvent(W_Display, &event);
    XPutBackEvent(W_Display, &event);
    W_SpNextEvent(wevent);
}

int
W_EventsPending()
{
    if (W_isEvent)
	return (1);
    while (XPending(W_Display) || buttonDown) {
	if (W_SpNextEvent(&W_myevent)) {
	    W_isEvent = 1;
	    return (1);
	}
    }
    return (0);
}

void
W_NextEvent(wevent)
    W_Event *wevent;
{
    if (W_isEvent) {
	*wevent = W_myevent;
	W_isEvent = 0;
	return;
    }
    /*while (W_SpNextEvent(wevent) == 0);*/
}

 int
W_SpNextEvent(wevent)
    W_Event *wevent;
{
    XEvent  event;
    XKeyEvent *key;
    XButtonEvent *button;
    XExposeEvent *expose;
    XResizeRequestEvent *resize;
    KeySym keysym;

    char    ch;
    struct window *win;
#ifdef CONTINUOUS_MOUSE
    static W_Event buttonEvent;
    static int delaytime, cupd = -1;
#endif				/* CONTINUOUS_MOUSE */

#ifdef DEBUG
    printf("Getting an event...\n");
#endif
    key = (XKeyEvent *) & event;
    button = (XButtonEvent *) & event;
    expose = (XExposeEvent *) & event;
    resize = (XResizeRequestEvent *) & event;
    for (;;) {
	if (XPending(W_Display))
	    XNextEvent(W_Display, &event);
#ifdef CONTINUOUS_MOUSE
	else if (buttonDown) {
	    if (continuousMouse && allowContinuousMouse) {
		if (cupd != udcounter) {
		    cupd = udcounter;
		    if (delaytime == 0) {
			bcopy(&buttonEvent, wevent, sizeof(W_Event));
			delaytime = clickDelay;
		    } else {
			delaytime--;
			wevent->type = -1;
		    }
		} else
		    wevent->type = -1;
		exitInputLoop = 1;
	    } else {
		wevent->type = -1;
		buttonDown = 0;
	    }
	    return (1);
	}
#endif
	else
	    return (0);
	/*
	   printf("read an event %d\n", event.type);
	*/
	win = findWindow(key->window);
	if (win == NULL)
	    return (0);
	if ((event.type == KeyPress || event.type == ButtonPress) &&
	    win->type == WIN_MENU) {
	    if (key->y % (W_Textheight + MENU_PAD * 2 + MENU_BAR) >=
		W_Textheight + MENU_PAD * 2)
		return (0);
	    key->y = key->y / (W_Textheight + MENU_PAD * 2 + MENU_BAR);
	}
	switch ((int) event.type) {
	case LeaveNotify:	/* for message window -- jfy */
	    return (0);
	    break;
	case KeyPress:
	    if (key->state & ControlMask) {
		controlkey = 1;
		key->state &= ~ControlMask;
	    } else
		controlkey = 0;
	    if (key->state & BillsScrewyAltMask) {
		altkey = 1;
		key->state &= ~BillsScrewyAltMask;
	    } else
		altkey = 0;
	    if (XLookupString(key, &ch, 1, &keysym, NULL) > 0) {
		wevent->type = W_EV_KEY;
		wevent->Window = W_Window2Void(win);
		wevent->x = key->x;
		wevent->y = key->y;
		if (controlkey)
		    wevent->key = (int) ch + 128;
		else if (altkey)
		    wevent->key = (int) ch + 256;
		else
		    wevent->key = ch;
		return (1);
	    } else { /* trap arrow keys */
		if(keysym == XK_Left || keysym == XK_Right) {
		    wevent->type = W_EV_KEY;
		    wevent->Window = W_Window2Void(win);
		    wevent->x = key->x;
		    wevent->y = key->y;
		    switch(keysym) {
		      case XK_Left:
			wevent->key = (int)'b'+128; /* Left arrow --> ctrl-b */
			break;
		      case XK_Right:
			wevent->key = (int)'f'+128; /* Right arrow --> ctrl-f */
			break;
		    }
		    return (1);
		}
	    }
	    return (0);
	    break;
#ifdef AUTOKEY
	case KeyRelease:
	    if (XLookupString(key, &ch, 1, &keysym, NULL) > 0) {
		wevent->type = W_EV_KEY_OFF;
		wevent->Window = W_Window2Void(win);
		wevent->x = key->x;
		wevent->y = key->y;
		wevent->key = ch;
		return (1);
	    } else {
		if(keysym == XK_Right || keysym == XK_Left) {
		    wevent->type = W_EV_KEY_OFF;
		    wevent->Window = W_Window2Void(win);
		    wevent->x = key->x;
		    wevent->y = key->y;
		    switch(keysym) {
		      case XK_Left:
			wevent->key = (int)'b'+128; /* Left arrow --> ctrl-b */
			break;
		      case XK_Right:
			wevent->key = (int)'f'+128; /* Right arrow --> ctrl-f */
			break;
		    }
		    return (1);
		}
	    }		
	    return (0);
	    break;
#endif				/* AUTOKEY */
	case ButtonPress:
	    wevent->type = W_EV_BUTTON;
	    wevent->Window = W_Window2Void(win);
	    wevent->x = button->x;
	    wevent->y = button->y;
	    switch (button->button & 0xf) {
	    case Button3:
		wevent->key = W_RBUTTON;
		break;
	    case Button1:
		wevent->key = W_LBUTTON;
		break;
	    case Button2:
		wevent->key = W_MBUTTON;
		break;
	    }
	    if (key->state & ControlMask)
		wevent->key += 6;
	    if (key->state & ShiftMask)
		wevent->key += 3;
	    if (key->state & BillsScrewyAltMask)
		wevent->key += 12;	/* alt */
#ifdef CONTINUOUS_MOUSE
	    if (continuousMouse && allowContinuousMouse &&
		(wevent->Window == w || wevent->Window == mapw) &&
	    /*
	       buttonRepeatMask allows only certain buttons to repeat
	       [BDyess]
	    */
		(1 << (wevent->key) & buttonRepeatMask)) {
		buttonDown = 1;
		exitInputLoop = 1;
		delaytime = clickDelay;
		bcopy(wevent, &buttonEvent, sizeof(W_Event));
	    }
	    return (1);
	case ButtonRelease:
	    /* bcopy(&buttonEvent,wevent,sizeof(W_Event)); */
	    wevent->type = -1;
	    buttonDown = 0;
	    return (1);
	case MotionNotify:
	    /*
	       the !buttonDown ensures that if you press a button and then
	       press another, release just the second, and then move the
	       mouse that nothing happens.
	    */
	    if (!(continuousMouse && allowContinuousMouse) || !buttonDown) {
		wevent->type = -1;
		return (1);
	    }
	    wevent->type = W_EV_BUTTON;
	    wevent->Window = W_Window2Void(win);
	    wevent->x = button->x;
	    wevent->y = button->y;
	    wevent->key = buttonEvent.key;
	    bcopy(wevent, &buttonEvent, sizeof(W_Event));
	    if (cupd == udcounter)
		wevent->type = -1;
	    else
		cupd = udcounter;

	    return (1);
#else
	    return (1);
#endif				/* CONTINUOUS_MOUSE */
	case Expose:
	    if (expose->count != 0)
		return (0);
	    if (win->type == WIN_SCROLL) {
		redrawScrolling(win);
		return (0);
	    }
	    if (win->type == WIN_MENU) {
		redrawMenu(win);
		return (0);
	    }
	    if (win->type == WIN_BORDER) {
	        redrawBorder(win);
		return (0);
	    }
	    if (win == W_Void2Window(baseWin)) {
	        /* main window has a reverse border [BDyess] */
		/*redrawReversedBorder(win);*/
	    }
	    wevent->type = W_EV_EXPOSE;
	    wevent->Window = W_Window2Void(win);
	    return (1);
	case ResizeRequest:
	    resizeScrolling(win, resize->width, resize->height);
	    break;
	default:
	    return (0);
	    break;
	}
    }
}

