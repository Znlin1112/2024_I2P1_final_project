#ifndef game_scene_h
#define game_scene_h

#include "utility.h"

typedef struct _playerInformation {
	
	char name[50];
	int score;

}playerInformation;

//void save_score(const char* filename, playerInformation* score);

Scene create_game_scene(void);
Scene create_losing_scene(void);
Scene create_winning_scene(void);
//Scene create_shop_scene(void);

static void update_winning(void);
static void update_losing(void);
//static void update_shop(void);


extern ALLEGRO_MOUSE_STATE mouseState;
extern int remain_enemy;
extern int enemy_count;
extern float setting_volume;
extern poison_flag;

#endif /* game_scene_h */
