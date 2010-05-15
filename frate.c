/* frate.c -- Frame rate code. */

#include "config.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "defs.h"


#define FPS  30


/*****************************************************************************/
#if defined(linux)

/* ITIMER_REAL is used for the time base, in a "one-shot" mode.  SIGALRM is
   generated when it expires, but this signal is held blocked.  sigsuspend()
   waits for SIGALRM to go off, or lets it go immediately if it's already
   pending.  Then a new ITIMER_REAL is started.  If for any reason the main
   loop takes longer than the desired time, no catch-ups are attempted,
   further loops just run normal times.

   The time desired for each loop varies.  A normal kernel runs the clock at
   10 milliseconds, so to get 30 frames per second loops are run in the
   pattern 30, 30, 40 milliseconds for an average 33.3ms.  The pattern is
   generated based on sysconf(_SC_CLK_TCK) and FPS, and it should work for
   any values of those settings.  Essentially, CLK_TCK ticks per second are
   divided up into FPS many buckets, as evenly as possible.

   Linux ITIMER_REAL does the traditional Unix timing thing which is to
   round up the requested delay to get clock ticks, and add this to the
   current tick.  For example if "jiffies" tick is 12345 and an interval of
   35ms is requested the interval is converted to 4 ticks and the alarm goes
   off when jiffies tick 12349 is reached.  This pays no attention to how
   much of the current tick has elapsed when the request is made.

   select(), poll(), usleep() and nanosleep() do it differently, they don't
   start counting time until the end of the current tick (by adding one tick
   to the requested time).  See /usr/src/linux/kernel/sched.c
   sys_nanosleep() where it adds "(t.tv_sec || t.tv_nsec)".  In the example
   the alarm would go off at jiffies tick 12350.  As far as I can tell it's
   impossible for those functions to do a delay that goes only to the next
   tick boundary, and because that could well be wanted on a slowish system,
   ITIMER_REAL is used instead.

   An alternative to the code here, for x86 PCs at least, would be /dev/rtc,
   which can be set to give high resolution timer interrupts that can be
   counted and blocked on (with read()).  See
   /usr/src/linux/Documentation/rtc.txt.

 */

static struct itimerval  table[FPS];
static int               upto = 0;
static sigset_t          alrmonly;


static void sigalrm(int signum)
{
}

void init_framerate(void)
{
  /* create table[] times */
  {
    long  clktck, period, quot, rem, r, t;
    int   i;

    clktck = sysconf (_SC_CLK_TCK);
    if (clktck == -1)
      {
        perror ("sysconf _SC_CLK_TCK");
        exit (1);
      }
    period = 1000000 / clktck;

    quot = clktck / FPS;
    rem = clktck % FPS;
    r = 0;
    for (i = 0; i < FPS; i++)
      {
        t = quot;
        r += rem;
        if (r >= FPS)
          {
            t++;
            r -= FPS;
          }

        /* The actual tv_usec set up is just under "t" ticks because the
           kernel will round up the requested value. */
        table[i].it_value.tv_usec = t*period - period/2;

        /* printf ("table[%d] = %ld\n", i, table[i].it_value.tv_usec); */
      }
  }

  /* make SIGALRM blocked */
  {
    sigset_t  set;
    sigemptyset (&set);
    sigaddset (&set, SIGALRM);
    sigprocmask (SIG_BLOCK, &set, NULL);
  }

  /* install dummy SIGALRM handler */
  {
    struct sigaction  act;
    act.sa_handler = sigalrm;
    sigemptyset (&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction (SIGALRM, &act, NULL);
  }

  /* sigset with only SIGALRM unmasked */
  sigfillset (&alrmonly);
  sigdelset (&alrmonly, SIGALRM); 

  /* start initial timer */
  setitimer (ITIMER_REAL, &table[0], NULL);
}

void do_framerate(void)
{
  /* wait for SIGALRM */
  sigsuspend (&alrmonly);

  /* new timer */
  setitimer (ITIMER_REAL, &table[upto], NULL);
  if (++upto >= FPS)
    upto = 0;

  /* Enable this code to print how long each loop is taking.  Redirect
     output to a file so results aren't affected by the time taken to draw
     the screen.  First print is bogus because "t" starts out zero.  */
#if 0
  {
    static struct timeval  t;
    struct timeval n;
    long   d;
    gettimeofday (&n, NULL);
    d = 1000000*(n.tv_sec-t.tv_sec) + n.tv_usec-t.tv_usec;
    t = n;

    printf ("%ld\n", d);
    /* if (d < 29000 || d > 41000) printf ("BAD %ld\n", d); */
  }
#endif
}


/*****************************************************************************/
#else /* !defined(linux) */

/* Old code, using select(). */


static struct timeval  otv;

void init_framerate(void)
{
  gettimeofday(&otv, 0);
}

void do_framerate(void)
{
  static struct timeval  ut;
  struct timeval  tv;

  gettimeofday(&tv, 0);
  ut.tv_usec = UTIMER
    - (1000000 * (tv.tv_sec - otv.tv_sec) + (tv.tv_usec - otv.tv_usec));

  /* Checking u<2*UTIMER guards against a hang if the system time changes. */
  if (ut.tv_usec > 0 && ut.tv_usec < 2*UTIMER)
    (void) select (0, 0, 0, 0, &ut);

  gettimeofday(&otv, 0);

  /* Enable this code to print how long each loop is taking.  Redirect
     output to a file so results aren't affected by the time taken to draw
     the screen.  First print is bogus because "t" starts out zero.  */
#if 0
  {
    static struct timeval  t;
    struct timeval n;
    long   d;
    gettimeofday (&n, 0);
    d = 1000000*(n.tv_sec-t.tv_sec) + n.tv_usec-t.tv_usec;
    t = n;

    printf ("%ld\n", d);
  }
#endif
}

#endif
