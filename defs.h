#define VERSION "2.1.1"

/* Area for setting flags to enable changes by Bryce and Neelix */

/*#define ORIGINAL_XGALAGA */
/*#define IM_A_BIG_FAT_CHEATER*/

#ifndef ORIGINAL_XGALAGA
# define ENABLE_SHIP_WRAP
# define ENABLE_SPREAD_SHOT 1 
# define ENABLE_MACHINE_GUN 1
# define DISABLE_RESET_ON_DEATH
# define ACTIVATED_SHIELD
# define SHOW_SHIELD_BAR
# define NO_PRIZE_WHILE_DEAD
#endif

#define MAXTORPS 10
#define MINTORPS 3
#define TORPSPEED 12

#define ETORPSPEED 8

#define MINSPEED 3
#define MAXSPEED 8

#define TORPDELAY 5

#define WINWIDTH  winwidth  /* was 400 */
#define WINHEIGHT winheight /* was 500 */

#define NUMSTARS 30

#define MAXALIENS 60

#define TURNSPEED 10

#define UTIMER 33333

#define BONUSSHIPSCORE 50000

#define ABS(a)			/* abs(a) */ (((a) < 0) ? -(a) : (a))

#define LEFTKEY 1
#define RIGHTKEY 2
#define FIREKEY 4

#ifndef HAVE_RANDOM
# ifndef HAVE_LRAND48
#  define random() rand()
# else
#  define random() lrand48()
# endif
#endif

#ifdef ORIGINAL_XGALAGA
  #define NUMWEAPONS 3 
#else
 #define NUMWEAPONS 3 + ENABLE_SPREAD_SHOT + ENABLE_MACHINE_GUN
#endif
#define SINGLESHOT 0
#define DOUBLESHOT 1
#define TRIPLESHOT 2
#define SPREADSHOT 3
#define MACHINEGUN 4

#ifdef ORIGINAL_XGALAGA
# define PRIZECHANCE 30
#else
# define PRIZECHANCE 25
#endif

#define TORPCHANCE 60

#ifdef ORIGINAL_XGALAGA
# define SHIELDTIME 300
#else
# define STARTSHIELDS 0
# define MAXSHIELDS 800
# define SHIELDTIME 200
#endif



#define ALIENSHAPES 17
