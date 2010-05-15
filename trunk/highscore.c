/* $Id: highscore.c,v 1.2 1998/04/30 05:11:55 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>

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
#ifdef _AIX
#include <net/nh.h>
#else /* This is for ntohl, htonl.  
	 This file is correct for hpux and linux for sure, 
	 probably others as well. */
#include <netinet/in.h>
#endif

#include "Wlib.h"
#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "data.h"

/* Global score file is insecure and rather useless today. */
#define NO_GLOBAL_SCORES 1
#ifdef NO_GLOBAL_SCORES
#define NUM_GLOBAL_SCORES 0
#else
#define NUM_GLOBAL_SCORES 20
#endif
#define NUM_MY_SCORES 10

static char new_name[20];
static int nnpos=0;
static int thisplace = -1, my_thisplace = -1;

static struct high_score {
#ifdef NOSCOREHOGS
    long uid;
#endif
    char name[20];
    long score;
    long level;
} 
#ifndef NO_GLOBAL_SCORES
	global_scores[NUM_GLOBAL_SCORES],
#endif
	my_scores[NUM_MY_SCORES];

void undo_name()
{
    W_ClearArea(baseWin, 
		WINWIDTH/2-(20*W_Textwidth), 250,
		(40*W_Textwidth), W_Textheight*2);
}

void do_name()
{
    char buf[21];
    static int init = 0;

    if(!init) {
	strcpy(new_name, getUsersFullName());
	nnpos = strlen(new_name);
	init = 1;
    }
    center_text("Great score! Enter your name:", 250, W_Red);
    sprintf(buf, "%s_", new_name);
    center_text(buf, 250 + W_Textheight, W_Cyan);
}

char *getUsersFullName()
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

static void save_scores()
{
    int i;
    int hsf;
    long x;
    char my_file_name [256], *home;
#ifndef NO_GLOBAL_SCORES
    hsf = open(SCOREFILE, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(hsf < 0) {
	printf("Couldn't write scores file %s\n", SCOREFILE);
	return;
    }

    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
#ifdef NOSCOREHOGS
        x=htonl(global_scores[i].uid);
	if(write(hsf, &x, sizeof(long)) < sizeof(long))
	    goto error;
#endif
	if(write(hsf, global_scores[i].name, 20) < 20)
	    goto error;
	x=htonl(global_scores[i].score);
	if(write(hsf, &x, sizeof(long)) < sizeof(long))
	    goto error;
	x=htonl(global_scores[i].level);
	if(write(hsf, &x, sizeof(long)) < sizeof(long))
	    goto error;
    }
    close(hsf);
#endif /* NO_GLOBAL_SCORES */
    if((home = getenv("HOME"))) {
#ifndef NO_GLOBAL_SCORES
	gid_t my_egid = getegid();
	if (setgid(getgid()) != 0) {
		perror("setgid");
		return;
	}
#endif
	snprintf(my_file_name, sizeof(my_file_name)-1, "%s/.xgalscores", home);
	hsf = open(my_file_name, O_WRONLY | O_TRUNC | O_CREAT, 0644);
#ifndef NO_GLOBAL_SCORES
	if (setgid(my_egid) != 0) {
		perror("setgid back");
		exit(1);
	}
#endif
	if(hsf < 0) {
	    printf("Couldn't write scores file %s\n", my_file_name);
	    return;
	}
	for(i=0;i<NUM_MY_SCORES;i++) {
	    if(write(hsf, my_scores[i].name, 20) < 20)
		goto error2;
	    x=htonl(my_scores[i].score);
	    if(write(hsf, &x, sizeof(long)) < sizeof(long))
		goto error2;
	    x=htonl(my_scores[i].level);
	    if(write(hsf, &x, sizeof(long)) < sizeof(long))
		goto error2;
	}
        close(hsf);
    }
	
    return;
#ifndef NO_GLOBAL_SCORES
  error:
    printf("Error saving high scores file %s\n", SCOREFILE);
    return;
#endif
  error2:
    printf("Error saving high scores file %s\n", my_file_name);
    return;
}

void add_score(char *name, int score)
{
    int i,j ; /* ,k; */

    thisplace = my_thisplace = -1;

    load_scores();

#ifndef NO_GLOBAL_SCORES
#ifdef NOSCOREHOGS
    /* /lib 27jul95: want to allow only one global high-score per person,
     * by uid (getuid)
     */
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
	if(score > global_scores[i].score) {
	    /* /lib 27jul95 uniq-global-highscore: */
	    /* find old position of high-score: */
	    for(k=i;k<NUM_GLOBAL_SCORES;k++) {
		if(global_scores[k].uid == getuid()) {
		    break;
		}
	    }
	    if(k==NUM_GLOBAL_SCORES) {
		k--;
	    }
	    /* found old pos, copy from i..k-1 to i+1..k */
	    for(j=k;j>i;j--) {
		global_scores[j].uid = global_scores[j-1].uid;
		strcpy(global_scores[j].name, global_scores[j-1].name);
		global_scores[j].score = global_scores[j-1].score;
		global_scores[j].level = global_scores[j-1].level;
	    }
	    global_scores[i].uid = getuid();
	    strcpy(global_scores[i].name, name);
	    global_scores[i].score = score;
	    global_scores[i].level = level;
	    thisplace = i;
	    break;
	}
	/* high-score stuff by /lib (uid is found, score is too low): */
	else if(global_scores[i].uid == getuid()) {
	    break;
	}
    }

#else /* NOSCOREHOGS */
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
	if(score > global_scores[i].score) {
	    for(j=NUM_GLOBAL_SCORES-1;j>i;j--) {
		strcpy(global_scores[j].name, global_scores[j-1].name);
		global_scores[j].score = global_scores[j-1].score;
		global_scores[j].level = global_scores[j-1].level;
	    }
	    strcpy(global_scores[i].name, name);
	    global_scores[i].score = score;
	    global_scores[i].level = level;
	    thisplace = i;
	    break;
	}
    }
#endif /* NOSCOREHOGS */
#endif /* NO_GLOBAL_SCORES */

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

int score_key(W_Event *ev)
{
    if(getting_name) {
	switch(ev->key) {
	  case 13:
	  case 10:
          case 269:
	    getting_name = 0;
	    add_score(new_name, score);
	    title_page = 1;
	    pagetimer = 300;
	    W_ClearWindow(baseWin);
	    break;
	  case 8:
	  case 127:
          case 264:
	    if(nnpos > 0) {
		nnpos--;
		new_name[nnpos] = 0;
	    }
	    break;
	  case 'u'+128:
	    nnpos = 0;
	    new_name[nnpos] = 0;
	    break;
	  default:
	    if(nnpos < 19) {
		new_name[nnpos++] = ev->key;
		new_name[nnpos] = 0;
	    }
	    break;
	}

	return 1;
    }
    return 0;
}


int check_score(int score)
{
    int i;

    load_scores(); /* in case someone else has gotten a high score */
#ifndef NO_GLOBAL_SCORES
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
	if(score > global_scores[i].score) {
	    return 1;
	}
#ifdef NOSCOREHOGS
 	if(global_scores[i].uid == getuid()) {
	    break;
 	}
#endif
    }
#endif /* NO_GLOBAL_SCORES */
    for(i=0;i<NUM_MY_SCORES;i++) {
	if(score > my_scores[i].score)
	    return 1;
    }

    my_thisplace = -1;
    thisplace = -1;
    return 0;
}


void show_scores()
{
    int i;
    char buf[60];
    W_Color color;

    W_SetRGB16(W_DarkGrey, random()%65535,random()%65535,random()%65535);

    color = W_DarkGrey;

    
#ifndef NO_GLOBAL_SCORES
    center_text("Global high scores", 90, W_Yellow);
    sprintf(buf, "Rank  Name                      Score   Level");
    center_text(buf, 100, W_Yellow);
    W_MakeLine(baseWin, WINWIDTH/2-((strlen(buf)*W_Textwidth)/2), 111,
	       WINWIDTH/2 + ((strlen(buf)*W_Textwidth)/2), 111, W_Red);
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
	sprintf(buf, "  %2d. %-20s     %7ld %5ld", i+1, 
		global_scores[i].name, global_scores[i].score,global_scores[i].level);
	center_text(buf, 112+i*W_Textheight, (i==thisplace ? color : W_Grey));
    }
#endif
    
    center_text("Your high scores", 112+NUM_GLOBAL_SCORES*W_Textheight, W_Yellow);
    sprintf(buf, "Rank  Name                      Score   Level");
    center_text(buf, 112+(NUM_GLOBAL_SCORES+1)*W_Textheight, W_Yellow);
    W_MakeLine(baseWin, WINWIDTH/2-((strlen(buf)*W_Textwidth)/2), 123+(NUM_GLOBAL_SCORES+1)*W_Textheight,
	       WINWIDTH/2 + ((strlen(buf)*W_Textwidth)/2), 123+(NUM_GLOBAL_SCORES+1)*W_Textheight, W_Red);
    for(i=0;i<NUM_MY_SCORES;i++) {
	sprintf(buf, "  %2d. %-20s     %7ld %5ld", i+1, 
		my_scores[i].name, my_scores[i].score,my_scores[i].level);
	center_text(buf, 124+(NUM_GLOBAL_SCORES+1)*W_Textheight + i*W_Textheight, 
		    (i==my_thisplace ? color : W_Grey));
    }
}

void load_scores()
{
    int i;
    int hsf;
    char my_file_name[256], *home;

#ifndef NO_GLOBAL_SCORES
    hsf = open(SCOREFILE, O_RDONLY);
    if(hsf <0 ) {
	printf("Trouble opening high scores file '%s'\n", SCOREFILE);
	for(i=0;i<NUM_GLOBAL_SCORES;i++) {
#ifdef NOSCOREHOGS
	    global_scores[i].uid = 0;
#endif
	    global_scores[i].name[0]=0;
	    global_scores[i].score = 0;
	    global_scores[i].level = 0;
	}
    } else {
	for(i=0;i<NUM_GLOBAL_SCORES;i++) {
#ifdef NOSCOREHOGS
	    if(read(hsf, &global_scores[i].uid, sizeof(long)) < sizeof(long))
		goto error;
#endif
	    if(read(hsf, global_scores[i].name, 20) < 20)
		goto error;
	    if(read(hsf, &global_scores[i].score, sizeof(long)) < sizeof(long))
		goto error;
	    if(read(hsf, &global_scores[i].level, sizeof(long)) < sizeof(long))
		goto error;
#ifdef NOSCOREHOGS
	    global_scores[i].uid = ntohl(global_scores[i].uid);
#endif
	    global_scores[i].score = ntohl(global_scores[i].score);
 	    global_scores[i].level = ntohl(global_scores[i].level);
	}
    }
    close(hsf);
#endif /* NO_GLOBAL_SCORES */

    if((home = getenv("HOME"))) {
	snprintf(my_file_name, sizeof(my_file_name)-1, "%s/.xgalscores", home);
	hsf = open(my_file_name, O_RDONLY);
	if(hsf <0 ) {
	    printf("Trouble opening high scores file '%s'\n", my_file_name);
	    for(i=0;i<NUM_MY_SCORES;i++) {
		my_scores[i].name[0]=0;
		my_scores[i].score = 0;
		my_scores[i].level = 0;
	    }
	} else {
	    for(i=0;i<NUM_MY_SCORES;i++) {
		if(read(hsf, my_scores[i].name, 20) < 20)
		    goto error2;
		if(read(hsf, &my_scores[i].score, sizeof(long)) < sizeof(long))
		    goto error2;
		if(read(hsf, &my_scores[i].level, sizeof(long)) < sizeof(long))
		    goto error2;
		my_scores[i].score = ntohl(my_scores[i].score);
		my_scores[i].level = ntohl(my_scores[i].level);
	    }
	}
	close(hsf);
    } else {
	printf("No HOME variable, so no personal score file.\n");
	for(i=0;i<NUM_MY_SCORES;i++) {
	    my_scores[i].name[0]=0;
	    my_scores[i].score = 0;
	    my_scores[i].level = 0;
	}
    }
    return;
#ifndef NO_GLOBAL_SCORES
  error:
    if(i>0)
	printf("Error reading high scores file '%s'\n", SCOREFILE);
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
#ifdef NOSCOREHOGS
	global_scores[i].uid = 0;
#endif
	global_scores[i].name[0]=0;
	global_scores[i].score = 0;
	global_scores[i].level = 0;
    }
    close(hsf);
    return;
#endif
  error2:
    if(i>0)
	printf("Error reading high scores file '%s'\n", my_file_name);
    for(i=0;i<NUM_MY_SCORES;i++) {
	my_scores[i].name[0]=0;
	my_scores[i].score = 0;
	my_scores[i].level = 0;
    }
    close(hsf);
}

void print_scores() {
    int i;
    
    load_scores();
#ifndef NO_GLOBAL_SCORES
    printf("\nGlobal High Scores:\n");
    printf("-----------------------------------------------\n");
    printf("%8s %-20s %8s %8s\n", "uid", "name", "score", "level");
    printf("-----------------------------------------------\n");
    for(i=0;i<NUM_GLOBAL_SCORES;i++) {
	if(global_scores[i].score == 0)
	    break;
#ifdef NOSCOREHOGS
	printf("%8ld %-20s %8ld %8ld\n", global_scores[i].uid, 
	       global_scores[i].name, global_scores[i].score, 
	       global_scores[i].level);
#else
	printf("      -- %-20s %8ld %8ld\n",
	       global_scores[i].name, global_scores[i].score,
	       global_scores[i].level);
#endif
    }
#endif /* NO_GLOBAL_SCORES */
    printf("-----------------------------------------------\n");
    printf("\nYour High Scores:\n");
    printf("--------------------------------------\n");
    printf("%-20s %8s %8s\n", "name", "score", "level");
    printf("--------------------------------------\n");
    for(i=0;i<NUM_MY_SCORES;i++) {
	if(my_scores[i].score == 0)
	    break;
	printf("%-20s %8ld %8ld\n", my_scores[i].name, 
	       my_scores[i].score, my_scores[i].level);
    }
    printf("--------------------------------------\n");
}
