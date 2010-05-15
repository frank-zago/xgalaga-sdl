/* images.h.  Contains the #defines and extern's for all the image management
   data [BDyess] */
/* automatically created by scripts/mkimgsrc */

#include "Wlib.h"

W_Image * getImage P((int offset));
void loadImageByFilename P((char *filename));
void loadAllImages P((void));

#define I_DEFAULT              0
#define I_AL                   0
#define I_ALIEN1               1
#define I_ALIEN10              2
#define I_ALIEN11              3
#define I_ALIEN12              4
#define I_ALIEN13              5
#define I_ALIEN14              6
#define I_ALIEN15              7
#define I_ALIEN16              8
#define I_ALIEN17              9
#define I_ALIEN2               10
#define I_ALIEN3               11
#define I_ALIEN4               12
#define I_ALIEN5               13
#define I_ALIEN6               14
#define I_ALIEN7               15
#define I_ALIEN8               16
#define I_ALIEN9               17
#define I_ETORP                18
#define I_EXPLOSION            19
#define I_EXTRA                20
#define I_MINISHIP             21
#define I_MTORP                22
#define I_PAUSE                23
#define I_PLAYER1              24
#define I_PLAYER2              25
#define I_PLAYER3              26
#define I_PR_BLANK             27
#define I_PR_BRAIN             28
#define I_PR_DOUB              29
#define I_PR_EXTRABULLET       30
#define I_PR_LEMON             31

#ifndef ORIGINAL_XGALAGA
#define I_PR_MACHINE           32
#define I_PR_SHIELD            33
#define I_PR_SING              34
#define I_PR_SPEED             35
#define I_PR_SPREAD            36
#define I_PR_TRIP              37
#define I_S1000                38
#define I_S2000                39
#define I_S4000                40
#define I_S500                 41
#define I_SHIELD               42
#define I_TITLE                43
#define I_LAST                 44
#else
#define I_PR_SHIELD            32
#define I_PR_SING              33
#define I_PR_SPEED             34
#define I_PR_TRIP              35
#define I_S1000                36
#define I_S2000                37
#define I_S4000                38
#define I_S500                 39
#define I_SHIELD               40
#define I_TITLE                41
#define I_LAST                 42
#endif /* ORIGINAL_XGALAGA */

