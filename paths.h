#ifndef PATHS_H
#define PATHS_H

/* enter_paths are for new ships.  consists of starting co-ordinates, number of frames
   to wait at the start (presumably offscreen) and a path to follow.
   path -1 means this ship is not used on this level
   at end of path, ship flies into convoy position on it's own from wherever it winds up
*/

struct enter_path {
    int shape, x, y, delay, path;
};

#define NUMLEVELS 15

#define P_PEELLEFT 0
#define P_PEELRIGHT 1
#define P_LOOP 2
#define P_SWOOP1 3
#define P_SWOOP2 4
#define P_ZIGZAG 5
#define P_LOOP2  6
#define P_SPIN   7
#define P_LEFTDIAG 8
#define P_RIGHTDIAG 9
#define P_ENTER1 10
#define P_ENTER2 11
#define P_ENTER3 12
#define P_ENTER4 13
#define P_ENTER5 14
#define P_ENTER6 15
#define P_ENTER7 16
#define P_ENTER8 17
#define P_ENTER9 18
#define P_ENTER10 19
#define P_ENTER11 20
#define P_ENTER12 21
#define P_ENTER13 22
#define P_ENTER14 23
#define P_ENTER15 24
#define P_ENTER16 25
#define P_ENTER17 26
#define P_ENTER18 27
#define P_ENTER19 28
#define P_ENTER20 29

#define NUMPATHS 30

#endif
