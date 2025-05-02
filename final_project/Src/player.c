#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);
ALLEGRO_COLOR tinted;
int poison_CD;



Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };

    player.animation_tick = 0;
	player.death_animation_tick = 0;
	player.poison_timer = 0;
    
    player.speed = 4;
    player.health = 100;
    
    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }

    poison_CD = 0;
	poison_flag = false;
    
    return player;
}

void update_player(Player * player, Map* map){

    /*
	if(poison_flag)
	    game_log("poison flag: %s", "poison flag TRUE");
	game_log("poison timer: %d", player->poison_timer);
    */

    Point original = player->coord;
    bool walk = false;
    
    // Knockback effect
    if(player->knockback_CD > 0){

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){next_x, player->coord.y};
        
        if(!isCollision(player, map)){
            player->coord = next;
        }
        
        next = (Point){player->coord.x, next_y};
        if(!isCollision(player, map)){
            player->coord = next;
        }
    }
    
    /*
        [OK TODO HACKATHON 1-1] 
        
        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */

    if (player->status != PLAYER_DYING) {

		//poison effect
        if (poison_flag) {
            player->poison_timer =  4 * 64;
            player->speed = 1.5;
            poison_flag = false;
        }
        if (player->poison_timer > 0) {
            if (player->poison_timer % 64 == 0 && poison_CD <= 0) {
                player->health -= 5;
				poison_CD = 32;
            }
			if (poison_CD > 0) {
				poison_CD--;
			}

            player->poison_timer--;
            player->speed = 1.5;
        }
        else {
			if (player->speed == 1.5)
                player->speed = 4;
        }



        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y -= player->speed;
            player->direction = DOWN;
            walk = true;
        }

        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y += player->speed;
            player->direction = UP;
            walk = true;
        }

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.x -= player->speed;
            player->direction = LEFT;
            walk = true;
        }

        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.x += player->speed;
            player->direction = RIGHT;
            walk = true;
        }

        if (walk) {
            player->status = PLAYER_WALKING;
        }
        else {
            player->status = PLAYER_IDLE;
        }

        if (isCollision(player, map)) {
            player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
        }

        if (isCollision(player, map)) {
            player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
        }
        player->animation_tick = (player->animation_tick + 1) % 64;

        if (player->health <= 0) {

            player->health = 0;
            player->status = PLAYER_DYING;

        }

    }
    else {
		player->death_animation_tick = (player->death_animation_tick + 1) % 64;
        if (player->death_animation_tick >= 63) {
            change_scene(create_losing_scene());
        }
    }    
    
    // if Collide, snap to the grid to make it pixel perfect
    
    /*
        [OK TODO HACKATHON 1-2] 
        
        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */
    
    /*
        [OK TODO Homework] 
        
        Calculate the animation tick to draw animation later
    */
    
    //game_log("health: %d", player->health);
}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    int flag = 0; // Change the flag to flip character
    
    /*
        [TODO Homework] 
        
        Draw Animation of Dying, Walking, and Idle
    */

    int num_tint_red = player->knockback_CD > 0 ? 120 : 255;

    if (player->poison_timer > 0) {
        tinted = al_map_rgb(186, 85, 211);
	}
	//else if (player->knockback_CD > 0) {
	//	tinted = al_map_rgb(255, 0, 0);
	//}
	else {
		tinted = al_map_rgb(255, num_tint_red, num_tint_red);
	}

    if (player->status == PLAYER_IDLE) {

        int offset = 32 * (int)(player->animation_tick / 32);    
        //calculate current frame to display
        //animation_tick = [0 , 63]


        //game_log("player IDEL: %d", offset);
        flag = player->direction == RIGHT ? 1 : 0;

        if (player->player_type == PLAYER_DEFAULT) {
            al_draw_tinted_scaled_bitmap(player->image, tinted,
                offset, 0, 32, 32, // source image x, y, width, height
                dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
                flag // Flip or not
            );
        }
    }
    else if (player->status == PLAYER_WALKING) {
        int offset = 32 * (int)(player->animation_tick / 16);

        //game_log("player WALKING: %d", offset);
        flag = player->direction == RIGHT ? 1 : 0;

        if (player->player_type == PLAYER_DEFAULT) {
            al_draw_tinted_scaled_bitmap(player->image, tinted,
                offset, 32, 32, 32, // source image x, y, width, height
                dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
                flag // Flip or not
            );
        }
    }
    else if (player->status == PLAYER_DYING) {   
        int offset = 32 * (int)(player->death_animation_tick / 16);
		flag = player->direction == RIGHT ? 1 : 0;

        if (player->player_type == PLAYER_DEFAULT) {
            al_draw_tinted_scaled_bitmap(player->image, tinted,
                offset, 64, 32, 32, // source image x, y, width, height
                dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
                flag // Flip or not
            );
        }
    }

    /*
    al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 255, 255),
        0, 0, 32, 32, // source image x, y, width, height
        dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
        flag // Flip or not
    );
    */
    

    
//#ifdef DRAW_HITBOX
//    al_draw_rectangle(
//        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
//        al_map_rgb(255, 0, 0), 1
//    );
//#endif
    
}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map){

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*

        [OK TODO HACKATHON 2-1] 
    
        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;

    */

    if (!isWalkable(map->map[player->coord.y / TILE_SIZE][player->coord.x / TILE_SIZE])) return true;
    if (!isWalkable(map->map[player->coord.y/ TILE_SIZE][(player->coord.x + TILE_SIZE - 1) / TILE_SIZE])) return true;
    if (!isWalkable(map->map[(player->coord.y + TILE_SIZE - 1) / TILE_SIZE][player->coord.x / TILE_SIZE])) return true;
    if (!isWalkable(map->map[(player->coord.y + TILE_SIZE - 1) / TILE_SIZE][(player->coord.x + TILE_SIZE - 1) / TILE_SIZE])) return true;


    
    return false;
}

void hitPlayer(Player * player, Point enemy_coord, int damage){
    if (player->knockback_CD == 0) {
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /*
            [OK TODO Homework]

            Game Logic when the player get hit or die

            player->knockback_angle = angle;
            player->knockback_CD = 32;

            player->health = ...
            if(player->health <= 0){

                player->health = 0;
                player->status = ...

                ...
            }
        */

        player->knockback_angle = angle;
        player->knockback_CD = 32;

        player->health -= damage;

        /*if (player->health <= 0) {

            player->health = 0;
            player->status = PLAYER_DYING;

        } */           
    }
}
