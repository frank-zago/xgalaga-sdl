/* $Id: pathfile.c,v 1.3 1998/04/30 05:11:56 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"
#include <config.h>


#include <stdio.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>

#include "struct.h"
#include "paths.h"
#include "pathfile.h"
#include "data.h"
#include "defs.h"

#define MAXFILENAME 1024

#define MAXPATHLEN 50
#define MAXLINE 1024

typedef enum {
    PF_PATHDEF,
    PF_PATHS,
    PF_SHAPES,
    PF_DELAYS
} PathfileToken;

struct path_entry
{
    int dir, duration;
};

struct path_info
{
    int startx, starty, len;
};

struct path_entry epaths[NUMPATHS][MAXPATHLEN];
struct path_info  pathinfo[NUMPATHS];

int al_shapes[MAXALIENS];
int al_delays[MAXALIENS];
int al_paths[MAXALIENS];

int
get_line(FILE* file, char* buf)
{
    int done = 0;
    char nextline[MAXLINE];

    buf[0] = 0;
    while(!done)
    {
	if(!fgets(nextline, MAXLINE, file))
	    return 0;

	/* remove newline */
	nextline[strlen(nextline) - 1] = 0;

	if(strlen(nextline) + strlen(buf) >= MAXLINE) {
	    fprintf(stderr, "get_line: Line too long\n");
	    return -1;
	}
	
	strcpy(buf + strlen(buf), nextline);

	if(nextline[strlen(nextline) -1] != '\\')
	    done = 1;
	else {
	    buf[strlen(buf) -1] = ' ';
	}
    }
    return 1;
}

PathfileToken
get_token(char* line, int* elem, char** data)
{
    PathfileToken pt;
    char *datastart;

    *elem = -1;
    
    if(strncmp(line, "paths:", 6) == 0) {	
	pt = PF_PATHS;
    } else if (strncmp(line, "path.", 5) == 0) {
	if(line[5] >= '0' && line[5] <= '9')
	    *elem = line[5] - '0';
	else if(line[5] >= 'a' && line[5] <= 'z')
	    *elem = line[5] - 'a' + 10;
	else if(line[5] >= 'A' && line[5] <= 'Z')
	    *elem = line[5] - 'A' + 36;
	else {
	    fprintf(stderr, "%c is not a valid path identifier\n", line[5]);
	    return -1;
	}
	pt = PF_PATHDEF;
    } else if (strncmp(line, "delays:", 7) == 0) {
	pt = PF_DELAYS;
    } else if (strncmp(line, "shapes:", 7) == 0) {
	pt = PF_SHAPES;
    } else {
	return -1;
    }

    datastart = strchr(line, ':');
    if(!datastart)
	return -1;
    datastart++;

    while(isspace(*datastart))
	datastart++;

    if(!(*datastart))
	return -1;

    *data = datastart;
    return pt;

}

#define iscompass(c) (((c) == 'n') || ((c) == 'e') || ((c) == 's') || ((c) == 'w'))
#define CHKDIR(x) \
  curdir++; if(strcmp(x, dir) == 0) { gotdir = curdir; }

static void
parse_path(int pnum, char* pstr)
{
    char *fnd;
    char dir[40];
    int dp;
    int curdir = -1, gotdir;
    int duration;
    int curentry;
    int sx, sy;
    char *comma;

    curentry = 0;

    comma = strchr(pstr, ',');
    sx = atoi(pstr);
    sy = atoi(comma+1);
    fnd = strchr(pstr, ':');
    while(!iscompass(*fnd) && *fnd)
	fnd++;

    if(!(*fnd))
	fprintf(stderr, "No path info for path %d\n", pnum);

    while(*fnd) {
	dp = 0;
	while(iscompass(*fnd) && dp < 3) {
	    dir[dp] = *fnd;
	    fnd++;
	    dp++;
	}
	if(!isdigit(*fnd)) {
	    fprintf(stderr, "Error parsing path: %s\n", pstr);
	    return;
	}
	dir[dp] = 0;
	curdir = -1;
	gotdir = -1;

	CHKDIR("n");
	CHKDIR("nne");
	CHKDIR("ne");
	CHKDIR("ene");
	CHKDIR("e");
	CHKDIR("ese");
	CHKDIR("se");
	CHKDIR("sse");
	CHKDIR("s");
	CHKDIR("ssw");
	CHKDIR("sw");
	CHKDIR("wsw");
	CHKDIR("w");
	CHKDIR("wnw");
	CHKDIR("nw");
	CHKDIR("nnw");

	if(gotdir < 0) {
	    fprintf(stderr, "direction %s in path %d is not valid\n",
		    dir, pnum);
	}

	duration = atoi(fnd);
	
	epaths[pnum][curentry].dir = gotdir;
	epaths[pnum][curentry].duration = duration;
	curentry++;
	while(isdigit(*fnd) || isspace(*fnd))
	    fnd++;
    }
    epaths[pnum][curentry].dir = -1;
    epaths[pnum][curentry].duration = -1;

    pathinfo[pnum].len = curentry;
    pathinfo[pnum].startx = sx;
    pathinfo[pnum].starty = sy;
}

static int
parse_shapes(char* data)
{
    char* chk;
    int i;

    for(i = 0, chk = data; i < MAXALIENS; i++)
    {
	while(isspace(*chk)) chk++;
	if(!(*chk))
	    return 0;

	if(*chk >= '0' && *chk <= '9')
	    al_shapes[i] = *chk - '0';
	else if(*chk >= 'a' && *chk <= 'z')
	    al_shapes[i] = *chk - 'a' + 10;
	else if(*chk >= 'A' && *chk <= 'Z')
	    al_shapes[i] = *chk - 'A' + 10 + 26;
	else if(*chk == '-')
	    al_shapes[i] = -1;
	else {
	    fprintf(stderr, "'%c' is not a valid shape character\n", *chk);
	    al_shapes[i] = 0;
	}

	chk++;
    }

    return 1;
}

static int
parse_paths(char* data)
{
    char* chk;
    int i;

    for(i = 0, chk = data; i < MAXALIENS; i++)
    {
	while(isspace(*chk)) chk++;
	if(!(*chk))
	    return 0;

	if(*chk >= '0' && *chk <= '9')
	    al_paths[i] = *chk - '0';
	else if(*chk >= 'a' && *chk <= 'z')
	    al_paths[i] = *chk - 'a' + 10;
	else if(*chk >= 'A' && *chk <= 'Z')
	    al_paths[i] = *chk - 'A' + 10 + 26;
	else if(*chk == '-')
	    al_paths[i] = -1;
	else {
	    fprintf(stderr, "'%c' is not a valid shape character\n", *chk);
	    al_paths[i] = 0;
	}

	chk++;
    }

    return 1;
}

static int
parse_delays(char* data)
{
    char* chk;
    int i;

    for(i = 0, chk = data; i < MAXALIENS; i++)
    {
	while(isspace(*chk)) chk++;
	if(!(*chk))
	    return 0;

	al_delays[i] = atoi(chk);
	while(!isspace(*chk) && *chk) chk++;
    }

    return 1;
}

int
read_level(int lev)
{
    char filename[MAXFILENAME];
    char readline[MAXLINE];
    char *data;
    int  gotPaths = 0, gotShapes = 0, gotDelays = 0;
    int rlen;
    int elem;
    FILE* lf;
    PathfileToken tok;
    static int maxLevel = 1;
    static char *real_level_path = 0;
    struct stat statbuf;

    if(!real_level_path)
    {
	sprintf(filename, "%s", LEVELDIR);
	if(stat(filename, &statbuf) != 0 || !(statbuf.st_mode & S_IFDIR)) {
	    sprintf(filename, "./levels");
	    if(stat(filename, &statbuf) != 0 || !(statbuf.st_mode & S_IFDIR)) {
		fprintf(stderr, "Can't find level directory %s OR ./levels\n",
			XGALAGADIR);
		return -1;
	    }
	}
	real_level_path = malloc(strlen(filename) + 1);
	strcpy(real_level_path, filename);
    }

    sprintf(filename, "%s/level%d.xgl", real_level_path, lev);
    lf = fopen(filename, "r");
    if(!lf) {
	/* Dont use recursion, to avoid stack overflow if start level is
	 * very high (or the game is played for a _very_ long time. :-) 
	 * -- JEH */
	for (lev = lev - maxLevel; lev > 0 && ! lf ;lev = lev - maxLevel) {
		metaLevel++;
		/* real_level_path is already set, so.. */
		sprintf(filename, "%s/level%d.xgl", real_level_path, lev);
		lf = fopen(filename, "r");
	}
	
        //fprintf(stderr, "Can't open level file %s\n", filename);
	//return read_level(lev - maxLevel);
    }

    if(lev > maxLevel)
	maxLevel = lev;

    while(!feof(lf)) {
	if((rlen = get_line(lf, readline)) <= 0) {
	    if(gotShapes && gotPaths && gotDelays) {
		return 1;
	    }
	    fprintf(stderr, "Error reading level file %s:\n", filename);
	    if(!gotShapes)
		fprintf(stderr, "  Missing shapes\n");
	    if(!gotPaths)
		fprintf(stderr, "  Missing paths\n");
	    if(!gotDelays)
		fprintf(stderr, "  Missing delays\n");

	    fclose(lf);
	    return 0;
	}
	
	switch((tok = get_token(readline, &elem, &data))) {
	case PF_PATHDEF:
	    parse_path(elem, data);
	    break;
	case PF_SHAPES:
	    gotShapes = parse_shapes(data);
	    break;
	case PF_PATHS:
	    gotPaths = parse_paths(data);
	    break;
	case PF_DELAYS:
	    gotDelays = parse_delays(data);
	    break;
	default:
	    break;
	}
    }

    fclose(lf);
    
    if(gotShapes && gotPaths && gotDelays) {
	return 1;
    }

    return 0;
}

int get_path(int anum)
{
    return al_paths[anum];
}

void get_xy(int anum, int* x, int* y)
{
    *x = (WINWIDTH  * pathinfo[al_paths[anum]].startx) / 400;
    *y = (WINHEIGHT * pathinfo[al_paths[anum]].starty) / 500;
}

int get_delay(int anum)
{
    return al_delays[anum];
}

int get_dir(int pnum, int pos)
{
    return epaths[pnum][pos].dir;
}

int get_duration(int pnum, int pos)
{
    return epaths[pnum][pos].duration;
}

int get_shape(int anum)
{
    return al_shapes[anum];
}
