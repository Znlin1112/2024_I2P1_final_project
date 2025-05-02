#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"

#include <math.h>

Player player; // Player
Map map; // Map
enemyNode * enemyList; // Enemy List
BulletNode * bulletList; // Bullet List

ALLEGRO_FONT* font;

ALLEGRO_BITMAP* Losing;
ALLEGRO_BITMAP* Winning;

ALLEGRO_BITMAP* coin;
ALLEGRO_BITMAP* heart;
ALLEGRO_BITMAP* cure;
ALLEGRO_BITMAP* speed_medicine;
//ALLEGRO_BITMAP* slider_hovered;

playerInformation* information;

Button sliderButton; 
Button shopButton;
Button buyButton;
Button buyEffectButton;

//Button* sliderButton_hovered;


// Weapon
Weapon weapons[2];
int time_record;
int weapon_type;
int coins_obtained;
bool coin_dissappearing_animation_playing;
bool condition_flag;
bool first_acvatied = true;
bool is_paused = false;
//bool return_to_game = false;
bool bgm_changed;
int effect_timer = 0;
bool bgm_changed;
//float setting_volume = 0.3f;

static void init(void){
    
    remain_enemy = 0;
    initEnemy();
    weapon_type = 0;
    
    map = create_map("Assets/map0.txt", 0);
    //map = create_map("Assets/map1.txt", 0);

    player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    
    weapons[0] = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 10, 0);
	weapons[1] = create_weapon("Assets/sniper.png", "Assets/purple_bullet.png", 48, 16, 30, 1);
    
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/game_bgm.mp3");

    //font edit
    font = al_load_ttf_font("Assets/Minecraft.ttf", 32, 0);
    if (!font) {
        game_log("font ini failed");
    }

	//load image
    heart = al_load_bitmap("Assets/heart.png");
    coin = al_load_bitmap("Assets/coin_icon.png");
	Losing = al_load_bitmap("Assets/panda_lose.png");
    Winning =  al_load_bitmap("Assets/panda_win.png");
	cure = al_load_bitmap("Assets/cure.png");
	speed_medicine = al_load_bitmap("Assets/speed_medicine.png");
	information = (playerInformation*)malloc(sizeof(playerInformation));


    //sliderButton
	sliderButton = button_create(SCREEN_W - 200, 40, 180, 15, "Assets/slider.png", "Assets/slider_hovered.png");
	sliderButton.default_img = al_load_bitmap("Assets/slider.png");
	sliderButton.hovered_img = al_load_bitmap("Assets/slider_hovered.png");
	sliderButton.hovered = false;

    if (first_acvatied) {
        sliderButton.value = 0.3;
		setting_volume = 0.3;
        first_acvatied = false;
	}
    else
    {
		sliderButton.value = setting_volume;
    }

    //shopButton
    shopButton = button_create(10, 140, 60, 60, "Assets/shop_compressed.png", "Assets/shop_compressed_hovered.png");
    shopButton.default_img = al_load_bitmap("Assets/shop_compressed.png");
    shopButton.hovered_img = al_load_bitmap("Assets/shop_compressed_hovered.png");
    shopButton.hovered = false;

	//buyButton
	buyButton = button_create(SCREEN_W / 2 + 90, SCREEN_H / 2 - 10, 140, 80, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
	buyButton.default_img = al_load_bitmap("Assets/UI_Button.png");

	//buyEffectButton
    buyEffectButton = button_create(SCREEN_W / 2 + 90, SCREEN_H / 2 + 100, 140, 80, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    buyEffectButton.default_img = al_load_bitmap("Assets/UI_Button.png");

	coins_obtained = 0;    
    coin_dissappearing_animation_playing = false;
	condition_flag = false;
    time_record = 0;
    bgm_changed = false;
	shopButton.CD = 0;
	buyButton.CD = 0;
	buyEffectButton.CD = 0;
	bool bgm_changed = false;
	poison_flag = false;    //ok
}

/*
static void init_shop(void) {
    game_log("Initializing Shop Scene ...");
}
*/

static void update(void){

    if (keyState[ALLEGRO_KEY_ESCAPE]) {
        //return_to_game = true;
        is_paused = false;
    }


    if (is_paused) {

		update_button(&sliderButton);
        if (mouseState.x >= sliderButton.x && mouseState.x <= sliderButton.x + sliderButton.w &&
            mouseState.y >= sliderButton.y && mouseState.y <= sliderButton.y + sliderButton.h) {
            sliderButton.hovered = true;
            if (mouseState.buttons) {
                sliderButton.value = (float)(mouseState.x - sliderButton.x) / sliderButton.w;
                al_set_mixer_gain(al_get_default_mixer(), sliderButton.value);
				setting_volume = sliderButton.value;
            }
        }
        else {
            sliderButton.hovered = false;
        }

        update_button(&buyButton);
		update_button(&buyEffectButton);
        if (mouseState.x >= buyButton.x && mouseState.x <= buyButton.x + buyButton.w &&
            mouseState.y >= buyButton.y && mouseState.y <= buyButton.y + buyButton.h) {
            buyButton.hovered = true;
            if (mouseState.buttons && buyButton.CD <= 0) {

                buyButton.CD = 30;

                if (coins_obtained >= 5) {
                    coins_obtained -= 5;
                    player.health += 10;
                }
            }

        }
        else {
            buyButton.hovered = false;
        }

		if (mouseState.x >= buyEffectButton.x && mouseState.x <= buyEffectButton.x + buyEffectButton.w &&
			mouseState.y >= buyEffectButton.y && mouseState.y <= buyEffectButton.y + buyEffectButton.h) {
			buyEffectButton.hovered = true;
			if (mouseState.buttons && buyEffectButton.CD <= 0) {
				
                buyEffectButton.CD = 30;
				
                if (coins_obtained >= 10) {
					coins_obtained -= 10;
                    effect_timer = 200;
				}
			}
		}
		else {
			buyEffectButton.hovered = false;
		}

		if (buyButton.CD > 0)
            buyButton.CD--;

        return;
    }

    /*
        [TODO Homework]
        
        Change the scene if winning/losing to win/lose scene
    */

    update_player(&player, &map);    
    if (player.status == PLAYER_DYING) {
        //change_scene(create_losing_scene());
		//this part is in player.c file, scene chsnge after playing dying animation
    }    
	if ( /*(coins_obtained == map.total_coins && !coin_dissappearing_animation_playing)  || */ remain_enemy == 0) {		
		change_scene(create_winning_scene());
	}

    /*
    if (poison_flag) {
		player.poison_timer = 3 * 64;
		player.speed = 2;
		poison_flag = false;
	}
    */

    /*
    if (player.poison_timer > 0) {
        player.poison_timer--;
		if (player.poison_timer % 32 == 0)
            player.health -= 5;
    }
	else if (player.poison_timer <= 0) {
		if (player.speed == 2)
		    player.speed = 4;
	}
    */
    

    if (effect_timer > 0) {

        effect_timer--;
        player.speed = 6;

    }
	else if (effect_timer <= 0) {
		player.speed = 4;
	}

    if (keyState[ALLEGRO_KEY_1]) {
        weapon_type = 0;
    }
    if (keyState[ALLEGRO_KEY_2]) {
        weapon_type = 1;
    }
    
	update_button(&sliderButton);
    /*
	* old code
    * 
	if (sliderButton.hovered == true) {
        sliderButton.hovered = true;
	}
    else {
		sliderButton.hovered = false;
    }    


    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);
    */


	if (mouseState.x >= shopButton.x && mouseState.x <= shopButton.x + shopButton.w &&
		mouseState.y >= shopButton.y && mouseState.y <= shopButton.y + shopButton.h) {
		shopButton.hovered = true;
		if (mouseState.buttons && shopButton.CD <= 0) {
			shopButton.CD = 30;
			is_paused = true;
		}
	}
	else {
		shopButton.hovered = false;
	}


    Point Camera;
    /*
        [OK TODO HACKATHON 1-3]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
    */
    Camera.x = player.coord.x - (SCREEN_W / 2);
    Camera.y = player.coord.y - (SCREEN_H / 2);

    //Camera = (Point){ 0, 0 };

    updateEnemyList(enemyList, &map, &player);
    update_weapon(&weapons[weapon_type], bulletList, player.coord, Camera);
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &coins_obtained);
    time_record++;
	if (shopButton.CD > 0)
		shopButton.CD--;
}

//update scenes
//static void update_shop(void) {
//
//    is_paused = true;
//
   // update_button(&buyButton);
   // if (mouseState.x >= buyButton.x && mouseState.x <= buyButton.x + buyButton.w &&
   //     mouseState.y >= buyButton.y && mouseState.y <= buyButton.y + buyButton.h) {
   //     buyButton.hovered = true;
   //     if (mouseState.buttons) {
			//if (coins_obtained >= 5) {
			//	coins_obtained -= 5;
			//	player.health += 10;
			//}
   //         else {
			//	//al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER, "Not enough coins!");
   //         }
   //     }
   // }
   // else {
   //     buyButton.hovered = false;
   // }
//
//    if (keyState[ALLEGRO_KEY_ESCAPE]) {
//        return_to_game = true;
//		is_paused = false;
//    }
//}

static void update_losing(void) {
    if (keyState[ALLEGRO_KEY_ENTER]) {
        change_scene(create_menu_scene());
    }
}

static void update_winning(void) {
    if (keyState[ALLEGRO_KEY_ENTER]) {
        change_scene(create_menu_scene());
    }
}

//draw scenes

//static void draw_shop(void) {
//	update_shop();
//    al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(0, 0, 0, 70));
//
	/*draw_button(buyButton);
    al_draw_text(
        font,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2 + 160,
        SCREEN_H / 2 + 10 - 3 + buyButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BUY"
    );
	al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 + 160, SCREEN_H / 2 + 10 + buyButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BUY");
    al_draw_scaled_bitmap(cure, 0, 0, 24, 34, SCREEN_W / 2 - 200, SCREEN_H / 2 - 5, 44, 64, 0);
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 240, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER, "$5");
	al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER, "+ 10 HP");
	
    char money[10];
    sprintf_s(money, sizeof(money), "%02d", coins_obtained);
    al_draw_text(font, al_map_rgb(255, 255, 255), 100, 34, 0, money);
    al_draw_scaled_bitmap(coin, 0, 0, 16, 16, 0, 10, 80, 80, 0);

	al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 32, ALLEGRO_ALIGN_CENTER, "SHOP");
	al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 64, ALLEGRO_ALIGN_CENTER, "[ESC] GO BACK TO GAME");
	al_flip_display();*/
//}


static void draw_losing(void) {

    //static bool bgm_changed = false;
    condition_flag = true;

    if (!bgm_changed) {
        change_bgm("Assets/audio/losing.mp3");
        bgm_changed = true;
    }    

    al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "LOSER LOL");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 50, ALLEGRO_ALIGN_CENTER, "PRESS [ENTER] TO CONTINUE");
	al_draw_scaled_bitmap(Losing, 0, 0, 64, 64, SCREEN_W / 2 + 120, SCREEN_H / 2 - 100, 150, 150, 0);
    al_flip_display();
	if (shopButton.CD > 0) 
        shopButton.CD--;
    
}

static void draw_winning(void) {
    
    //static bool bgm_changed = false;
    condition_flag = true;

    if (!bgm_changed) {
        change_bgm("Assets/audio/win.mp3");
        bgm_changed = true;
    }

	//information->score = coins_obtained * 10 + enemy_count * 20 - (int)time_record * 0.00001;

    //save_score("scoreBoard.txt", information->score);

    //change_bgm("Assets/audio/win.mp3");
    al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "You WIN! Good job!");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 50, ALLEGRO_ALIGN_CENTER, "PRESS [ENTER] TO CONTINUE");
    al_draw_scaled_bitmap(Winning, 0, 0, 64, 64, SCREEN_W / 2 + 180, SCREEN_H / 2 - 105, 150, 150, 0);
    al_flip_display();
    
}

static void draw(void){

    Point Camera;

    /*
        [OK TODO HACKATHON 1-4]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [OK TODO HACKATHON 1-3]
    */

    Camera.x = player.coord.x - (SCREEN_W / 2);
    Camera.y = player.coord.y - (SCREEN_H / 2);
    
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
	if (player.status != PLAYER_DYING)
        draw_weapon(&(weapons[weapon_type]), player.coord, Camera);

    draw_button(shopButton);

    

    /*
        [TODO Homework]
        
        Draw the UI of Health and Total Coins
    */
    
    al_draw_scaled_bitmap(heart, 0, 0, 32, 32, 0, 0, 80, 80, 0);
    al_draw_scaled_bitmap(coin, 0, 0, 16, 16, 0, 60, 80, 80, 0);

    char HP[10], money[10];
    sprintf_s(HP, sizeof(HP), "%02d", player.health);
    sprintf_s(money, sizeof(money), "%02d", coins_obtained);
    al_draw_text(font, al_map_rgb(255, 255, 255), 100, 24, 0, HP);
    al_draw_text(font, al_map_rgb(255, 255, 255), 100, 86, 0, money);

    if (is_paused) {
        //background
        al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(0, 0, 0, 200));

		//buttons
        draw_button(buyButton);
        al_draw_text(
            font,
            al_map_rgb(66, 76, 110),
            SCREEN_W / 2 + 160,
            SCREEN_H / 2 + 10 - 3 + buyButton.hovered * 11,
            ALLEGRO_ALIGN_CENTER,
            "BUY"
        );
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 + 160, SCREEN_H / 2 + 10 + buyButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BUY");
        al_draw_scaled_bitmap(cure, 0, 0, 24, 34, SCREEN_W / 2 - 200, SCREEN_H / 2 - 5, 44, 64, 0);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 240, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER, "$5");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER, "+ 10 HP");

		draw_button(buyEffectButton);
		al_draw_text(
			font,
			al_map_rgb(66, 76, 110),
			SCREEN_W / 2 + 160,
			SCREEN_H / 2 + 120 - 3 + buyEffectButton.hovered * 11,
			ALLEGRO_ALIGN_CENTER,
			"BUY"
		);
		al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 + 160, SCREEN_H / 2 + 120 + buyEffectButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BUY");
		al_draw_scaled_bitmap(speed_medicine, 0, 0, 32, 32, SCREEN_W / 2 - 210, SCREEN_H / 2 + 120, 64, 64, 0);
		al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 240, SCREEN_H / 2 + 140, ALLEGRO_ALIGN_CENTER, "$10");
		al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 40, SCREEN_H / 2 + 140, ALLEGRO_ALIGN_CENTER, "Speed Up");

        draw_button(sliderButton);
        al_draw_filled_rectangle(sliderButton.x, sliderButton.y, sliderButton.x + sliderButton.w * sliderButton.value, sliderButton.y + sliderButton.h, al_map_rgb(255, 255, 255));
        al_draw_rounded_rectangle(sliderButton.x, sliderButton.y, sliderButton.x + sliderButton.w, sliderButton.y + sliderButton.h, 2, 2, al_map_rgb(58, 58, 58), 3);
        al_draw_filled_rectangle(sliderButton.x + sliderButton.w * sliderButton.value - 5, sliderButton.y - 5, sliderButton.x + sliderButton.w * sliderButton.value + 5, sliderButton.y + sliderButton.h + 5, al_map_rgb(177, 177, 177));
        al_draw_rounded_rectangle(sliderButton.x + sliderButton.w * sliderButton.value - 5, sliderButton.y - 5, sliderButton.x + sliderButton.w * sliderButton.value + 5, sliderButton.y + sliderButton.h + 5, 2, 2, al_map_rgb(58, 58, 58), 3);

        char volume[10];
        sprintf_s(volume, sizeof(volume), "%02d", (int)(100 * sliderButton.value));

        al_draw_text(
            P2_FONT,
            al_map_rgb(255, 255, 255),
            sliderButton.x - 15,
            sliderButton.y - 7,
            ALLEGRO_ALIGN_RIGHT,
            volume
        );

        //UI
        char HP[10], money[10];
        sprintf_s(HP, sizeof(HP), "%02d", player.health);
        sprintf_s(money, sizeof(money), "%02d", coins_obtained);
        al_draw_text(font, al_map_rgb(255, 255, 255), 100, 24, 0, HP);
        al_draw_text(font, al_map_rgb(255, 255, 255), 100, 86, 0, money);
        al_draw_scaled_bitmap(heart, 0, 0, 32, 32, 0, 0, 80, 80, 0);
        al_draw_scaled_bitmap(coin, 0, 0, 16, 16, 0, 60, 80, 80, 0);

        //text
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 32, ALLEGRO_ALIGN_CENTER, "SHOP");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 64, ALLEGRO_ALIGN_CENTER, "[ESC] GO BACK TO GAME");
        al_flip_display();
    }
}

/*
void load_scores(const char* filename, playerInformation* scores, int* count) {
    FILE* file = fopen(filename, "r");
    if (file) {
        *count = 0;
        while (fscanf(file, "%s %d", scores[*count].name, &scores[*count].score) == 2 && *count < MAX_SCORES) {
            (*count)++;
        }
        fclose(file);
    }
    else {
        game_log("Failed to open score file for reading");
    }
}
*/

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapons);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
	destroy_button(&sliderButton);
	destroy_button(&shopButton);
	destroy_button(&buyButton);
	destroy_button(&buyEffectButton);
    terminateEnemy();
}

/*
void save_score(const char* filename, playerInformation* score) {
    FILE* file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s %d\n", score->name, score->score);
        fclose(file);
    }
    else {
        game_log("Failed to open score file for recording scoreboard");
    }
}
*/

Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}

Scene create_losing_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "losing";
    scene.init = &init;
    scene.draw = &draw_losing;
    scene.update = &update_losing;
    scene.destroy = NULL;

    return scene;
}

Scene create_winning_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "Winning";
    scene.init = &init;
    scene.draw = &draw_winning;
    scene.update = &update_winning;
    scene.destroy = NULL;

    return scene;
}

//Scene create_shop_scene(void)*/ {
//    Scene scene;
//    memset(&scene, 0, sizeof(Scene));
//
//    scene.name = "Shop";
//    scene.init = &init;
//    scene.draw = &draw_shop;
//    scene.update = &update_shop;
//    scene.destroy = NULL;
//
//    return scene;
//}
