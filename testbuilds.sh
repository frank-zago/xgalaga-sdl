#!/bin/sh

# Test various build configurations. Both SDL 1 and 2 must be
# installed.

# Exit at the first error
set -e

# SDL 1 + TTF
autoreconf -i
./configure
make clean
make -j4

# SDL 1, no TTF
autoreconf -i
./configure --disable-ttf
make clean
make -j4

# SDL 2 + TTF
autoreconf -i
./configure --enable-sdl2
make clean
make -j4

# SDL 2, no TTF is not supported (yet?)

echo
echo Success
echo
