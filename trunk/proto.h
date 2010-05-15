#include <string.h>

/* main.c */
void xgal_exit(int v);
int main(int argc, char **argv);
/* images.c */
W_Image *getImage(int offset);
int cmpfilenames(const void *left, const void *right);
void loadImageByFilename(char *filename);
void loadAllImages(void);
/* data.c */
/* paths.c */
void path_dir(int path, int pos, int *dir, int *steer);
void enter_path_dir(int path, int pos, int *dir, int *steer);
void start_path(int path, struct alien *al);
void new_alien(int level, int i, struct alien *al);
/* title.c */
void undo_pause(void);
void do_pause(void);
void center_text(char *text, int y, W_Color color);
void do_title(void);
void init_titles(void);
/* explosions.c */
void undo_explosions(void);
void do_explosions(void);
void new_explosion(int x, int y, int type);
void score_flagship(int x, int y, int ne);
void init_explosions(void);
/* score.c */
void undo_score(void);
void do_score(void);
void draw_score(void);
void init_score(void);
/* highscore.c */
void undo_name(void);
void do_name(void);
char *getUsersFullName(void);
void add_score(char *name, int score);
int score_key(W_Event *ev);
int check_score(int score);
void show_scores(void);
void load_scores(void);
void print_scores(void);
/* prize.c */
void init_prizes(void);
void new_prize(int x, int y);
void undo_prizes(void);
void do_prizes(void);
/* sound.c */
void init_sound(void);
void play_sound(int k);
void maybe_play_sound(int k);
void sound_completed(int k);
void kill_sound(void);
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
/* linux-joystick.c */
void init_joystick(void);
void do_joystick(int* x, int* y, int* but);
