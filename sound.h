/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
 * Copyright (c) 1994 Sujal M. Patel (smpatel@wam.umd.edu)
 * Copyright (c) 2010 Frank Zago
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#define SND_EXPLOSION 0
#define SND_FIRETORP  1
#define SND_SHIELD    2
#define SND_TORPHIT   3
#define SND_EXP_SB    4
#define SND_DDLOO     5
#define SND_WARP      6
#define SND_SMART     7

void init_sound (void);			/* Init Sound System */
void play_sound (int k);		/* Play a Sound */
