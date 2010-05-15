# Makefile.in for xgalaga
# $Id: Makefile.in,v 1.4 1998/05/11 06:52:59 mrogre Exp $

#include config.h

top_srcdir = @top_srcdir@
srcdir = @srcdir@
VPATH = @srcdir@

.SUFFIXES:
.SUFFIXES: .c .o

prefix = @prefix@
exec_prefix = @exec_prefix@
bindir = $(exec_prefix)
libdir = $(prefix)/lib
infodir = $(prefix)/info
#mandir = @mandir@
mandir = /usr/share/man
creditsfile = @CREDITSFILE@
distversion = 2.1.1.0

@SET_MAKE@

SOUNDSRV = @SOUNDSERVER@
SOUNDSRVDIR = @SOUNDSRVDIR@
SOUNDOBJS = xgal.sndsrv.linux.o
SOUNDSRCS = xgal.sndsrv.linux.c

SOUNDDEFS = -DSOUNDSERVER=\"$(prefix)$(SOUNDSRVDIR)/@SOUNDSERVER@\"

RM = rm -f
CC = @CC@
CFLAGS = -I. @CFLAGS@ @DEFS@ @X_CFLAGS@ -DXGALAGADIR=\"@prefix@/\" $(SOUNDDEFS)
LDFLAGS = @LDFLAGS@
LIBS = libsprite/libsprite.a @LIBS@ @X_LIBS@
INSTALL = @INSTALL@

OBJS = main.o images.o data.o paths.o pathfile.o title.o explosions.o \
	score.o highscore.o prize.o sound.o nas_sound.o frate.o linux-joystick.o
OBJS_HYPER = $(patsubst %.o,%.o.hyper,$(OBJS))
SRCS = `echo $(OBJS) | sed -e 's/\.o/.c/g'`
MANPAGE = xgalaga.6x.gz

TARGET = xgalaga
TARGET2 = xgalaga-hyperspace

SUBDIRS = libsprite

all: $(TARGET) $(TARGET2) $(SOUNDSRV) $(MANPAGE)

$(SOUNDSRV): $(SOUNDOBJS)
	$(CC) $(LDFLAGS) -o $(SOUNDSRV) $(SOUNDOBJS) $(SOUNDLIBS) $(LIBOSSAUDIO)

install: all
	./mkinstalldirs $(DESTDIR)$(bindir)
	./mkinstalldirs $(DESTDIR)$(prefix)
	$(INSTALL) $(TARGET) $(DESTDIR)$(bindir)/$(TARGET)
	$(INSTALL) $(TARGET2) $(DESTDIR)$(bindir)/$(TARGET2)
	if test ! "$(SOUNDSRV)" = ""; then \
		$(INSTALL) $(SOUNDSRV) $(DESTDIR)$(prefix)$(SOUNDSRVDIR)/$(SOUNDSRV); \
	fi
	./mkinstalldirs $(DESTDIR)$(prefix)/sounds
	./mkinstalldirs $(DESTDIR)$(prefix)/levels
	for i in levels/*.xgl; do $(INSTALL) $$i $(DESTDIR)$(prefix)/$$i ; done
	for i in sounds/*.raw; do $(INSTALL) $$i $(DESTDIR)$(prefix)/$$i ; done
	$(INSTALL) CREDITS $(DESTDIR)$(creditsfile)
	$(INSTALL) xgalaga.6x.gz $(mandir)/man6/xgalaga.6x.gz
	chmod 644 $(mandir)/man6/xgalaga.6x.gz
#	touch $(prefix)/scores
#	chmod 666 $(prefix)/scores
#       chown ${GAMEOWN}:${GAMEGRP} $(prefix)/scores
#       chown ${GAMEOWN}:${GAMEGRP} $(bindir)/$(TARGET)
#       chmod ${GAMEMODE} $(bindir)/$(TARGET)
#       chmod ${GAMEDATAMODE} $(prefix)/scores

subdirs:
	for i in $(SUBDIRS); do cd $$i ; make ; done

$(TARGET): subdirs $(OBJS)
	$(RM) $(TARGET)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(TARGET2): subdirs $(OBJS_HYPER)
	$(RM) $(TARGET2)
	$(CC) $(LDFLAGS) -o $(TARGET2) $(OBJS_HYPER) $(LIBS)

%.6x.gz: %.6x
	gzip -c $< > $@

%.o: %.c
	$(CC) $(CFLAGS) -DORIGINAL_XGALAGA -c $< -o $@

%.o.hyper: %.c
	$(CC) $(CFLAGS) -UORIGINAL_XGALAGA -c $< -o $@

clean:
	$(RM) core *.o $(OBJS) $(OBJS_HYPER) $(TARGET) $(TARGET2) $(MANPAGE)

dist: clean
	cd libsprite;make clean
	if test -d xgalaga-$(distverson) ; then echo -n; else  mkdir xgalaga-$(distversion); fi
	cp -rf $(SRCS) configure *.in *.sub *.guess images sounds xgal.sndsrv.*.c libsprite README* INSTALL CHANGES CREDITS xgalaga.6x *.h install-sh levels mkimgsrc* mkinstalldirs xgalaga-$(distversion)
	cd xgalaga-$(distversion); rm -f config.h
	cd xgalaga-$(distversion)/libsprite; make distclean ; rm -rf CVS
	find xgalaga-$(distversion) -name "CVS" -exec rm -rf {} \; -prune
	tar czvf xgalaga-$(distversion).tar.gz xgalaga-$(distversion)

distclean: clean
	$(RM) Makefile config.h config.status config.cache config.log
	cd libsprite;$(MAKE) distclean

depend:
	makedepend $(CFLAGS) $(SRCS)
