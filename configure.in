dnl Process this file with autoconf to produce a configure script.
AC_INIT(XGalaga-SDL, 2.1.1.0-sdl)
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Enable compilation warnings if gcc is used.
if test "${ac_cv_c_compiler_gnu}" = "yes"; then
  CFLAGS="$CFLAGS \
    -ggdb \
    -Wall \
	-W \
    -Wcast-align \
    -Wcast-qual \
    -Wmissing-declarations \
    -Wmissing-prototypes \
    -Wpointer-arith \
    -Wreturn-type \
    -Wstrict-prototypes \
    "
fi # ac_cv_c_compiler_gnu

# Silent rules; but check if supported.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Check for SDL - Will accept 1.2 and 1.3
AM_PATH_SDL("1.2.0", :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))

# Check for SDL libraries
AC_CHECK_LIB(SDL, SDL_Init,,AC_MSG_ERROR([SDL library not found.]))
AC_CHECK_LIB(SDL_mixer,main,,AC_MSG_ERROR([SDL_mixer library not found.]))
AC_CHECK_LIB(SDL_image,IMG_Load,,AC_MSG_ERROR([SDL_image library not found.]))
AC_CHECK_LIB(SDL_gfx,SDL_framerateDelay,,AC_MSG_ERROR([SDL_gfx library not found.]))


AC_ARG_ENABLE(ttf,
  AS_HELP_STRING([--disable-ttf],[disable use of ttf fonts]),
  [ if test x"$enableval" = xyes; then
    with_ttf="yes, check"
  else
    with_ttf="no"
    problem_ttf=": Explicitly disabled"
  fi ],
  [ with_ttf="not specified, check" ]
)
if ! test x"$with_ttf" = xno; then
   AC_CHECK_LIB(SDL_ttf,TTF_Init,,AC_MSG_ERROR([SDL_ttf library not found.]))
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h malloc.h strings.h unistd.h)

dnl Checks for library functions.
AC_CHECK_FUNCS(random lrand48)

AC_SUBST(SDL_LIBS)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(datadir)

AC_CONFIG_HEADERS([src/config.h])

AC_OUTPUT(Makefile src/Makefile)
