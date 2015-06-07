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

# Check for SDL - Will accept 1.2 and 2
AC_ARG_ENABLE(sdl2,
  AS_HELP_STRING([--enable-sdl2], [use SDL2 instead of SDL1]),
  [ if test x"$enableval" = xyes; then
    with_sdl2="yes"
  else
    with_sdl2="no"
  fi ],
  [ with_sdl2="no" ]
)

AC_ARG_ENABLE(ttf,
  AS_HELP_STRING([--disable-ttf], [disable use of ttf fonts]),
  [ if test x"$enableval" = xyes; then
    with_ttf="yes"
  else
    with_ttf="no"
  fi ],
  [ with_ttf="yes" ]
)

PKG_PROG_PKG_CONFIG

if test x"$with_sdl2" = xyes; then
  PKG_CHECK_MODULES([SDL2], [sdl2, SDL2_image, SDL2_mixer, SDL2_gfx])
  if ! test x"$with_ttf" = xno; then
    PKG_CHECK_MODULES([SDL2_ttf], [SDL2_ttf])
  fi
else
  PKG_CHECK_MODULES([SDL], [sdl, SDL_image, SDL_mixer, SDL_gfx])
  if ! test x"$with_ttf" = xno; then
    PKG_CHECK_MODULES([SDL_ttf], [SDL_ttf])
  fi
fi

if ! test "x$with_ttf" = "xno"; then
   AC_DEFINE([HAVE_LIBSDLX_TTF], [1], [Build with TTF font support])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h malloc.h strings.h unistd.h)

dnl Checks for library functions.
AC_CHECK_FUNCS(random lrand48)

AC_SUBST(datadir)

AC_CONFIG_HEADERS([src/config.h])

AC_OUTPUT(Makefile src/Makefile)

dnl Display configuration
AC_MSG_RESULT([
        $PACKAGE $VERSION
        =====

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}

        SDL2:                   ${with_sdl2}
        SDL_TTF:                ${with_ttf}
])
