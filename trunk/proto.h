/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
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

#include <string.h>
#include <SDL/SDL.h>

/* images.c */
struct W_Image *getImage(int offset);
int loadAllImages(void);

/* paths.c */
void path_dir(int path, int pos, int *dir, int *steer);
void enter_path_dir(int path, int pos, int *dir, int *steer);
void start_path(int path, struct alien *al);
void new_alien(int level, int i, struct alien *al);

/* title.c */
void do_pause(void);
void do_title(void);
void init_titles(void);
void title_page_next(void);
void title_page_prev(void);

/* explosions.c */
void undo_explosions(void);
void do_explosions(void);
void new_explosion(int x, int y, int type);
void score_flagship(int x, int y, int ne);
void init_explosions(void);

/* score.c */
void do_score(void);
void draw_score(void);
void init_score(void);

/* highscore.c */
void do_name(void);
char *getUsersFullName(void);
void add_score(char *name, int score);
int score_key(SDLKey key);
int check_score(int score);
void show_scores(int top);
void load_scores(void);
void print_scores(void);

/* prize.c */
void init_prizes(void);
void new_prize(int x, int y);
void undo_prizes(void);
void do_prizes(void);
void show_bonuses(int top);

/* sound.c */
void init_sound(void);
void play_sound(int k);
void sound_exit(void);

/* pathfile.c */
int get_path(int);
void get_xy(int, int*, int*);
int get_delay(int);
int get_dir(int, int);
int get_duration(int, int);
int get_shape(int);
int read_level(int);

/* frate.c */
void init_framerate(void);
void do_framerate(void);

/* joystick.c */
void init_joystick(void);

/* sdl.c */
void S_Initialize(int fullscreen);
void S_DrawPoint (unsigned int x, unsigned int y, Uint32 pixel);
void S_ClearScreen(void);
void S_DrawImage (int x, int y, int frame,
				  struct W_Image *image);
void toggle_fullscreen(void);

//todo: remove
#define center_text(buf, y, font) SFont_WriteCenter((font), (y), (buf))

