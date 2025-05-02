#include "bullet.h"

#include <math.h>

//bullet done

Bullet create_bullet(char* bullet_path, PointFloat coord, float angle, float speed, float damage){
    Bullet bullet;
    bullet.coord = coord;
    bullet.angle = angle;
    bullet.speed = speed;
    bullet.damage = damage;
    bullet.image = al_load_bitmap(bullet_path);

    // For better repositioning
    bullet.coord.x += (float)(TILE_SIZE/2) * cos(bullet.angle);     ///tile means each block og the map
    bullet.coord.y += (float)(TILE_SIZE/2) * sin(bullet.angle);
    
    return bullet;
}

// Return true if the bullet collides, so it will be deleted from the list
bool update_bullet(Bullet * bullet, enemyNode * enemyList, Map * map){
    /*
        [OK TODO Hackathon 3-1] 
        
        Calculate the correct formula to update the bullet,
        Parameter: bullet->speed, bullet->angle

        bullet->coord.x += bullet->speed * cos(bullet->angle);
        bullet->coord.y += bullet->speed * sin(bullet->angle);
    */

    bullet->coord.x += bullet->speed * cos(bullet->angle);
    bullet->coord.y += bullet->speed * sin(bullet->angle);

    //bullet->coord.x += 4;     this cause a shift, what the hell
    //bullet->coord.y += 0;     uhhh default maybe
    
    /*
        [OK TODO Hackathon 2-3] 
        
        Return true if the bullet collides, so it will deleted from the lists

        int tile_y = (int)(bullet->coord.x / TILE_SIZE);
        int tile_x = (int)(bullet->coord.y / TILE_SIZE);
        if(tile_x < 0 || ...  || map->map[tile_x][tile_y] == ...){
            return true;
        }
    */

    //able to shoot
    int tile_y = (int)(bullet->coord.x / TILE_SIZE);
    int tile_x = (int)(bullet->coord.y / TILE_SIZE);
    if (tile_x >= map->col || tile_x >= map->row || tile_x < 0 || tile_y < 0 || !isWalkable(map->map[tile_x][tile_y])) {
        return true;
    }    
    
    
    // Check if the bullet collide with the enemies by simple iterating
    enemyNode * cur = enemyList->next;
    while(cur != NULL){
        Point enemyCoord = cur->enemy.coord;

        /*
            [OK TODO Hackathon 3-2] Hit the enemy
            
            Return true if the bullet hit the enemy so we can erase the bullet

            In here, we iterate list of enemies until we hit one of them

            Hint: The enemy coordinate is defined at enemyCoord variable,
            while their width & height are equal to TILE_SIZE

            if(...){
                hitEnemy(...);
                return true;
            }
        */


        int enemy_tile_y = (int)(enemyCoord.x / TILE_SIZE);
        int enemy_tile_x = (int)(enemyCoord.y/ TILE_SIZE);
        if (enemy_tile_y <= tile_y && enemy_tile_x <= tile_x && 
            enemy_tile_y + 1 >= tile_y && enemy_tile_x + 1 >= tile_x) {
            hitEnemy(&cur->enemy, bullet->damage, bullet->angle);
            return true;
        }       
        

        cur = cur->next;
    }
    return false;
}

void draw_bullet(Bullet * bullet, Point camera){
    float scale = TILE_SIZE / 16;
    //al_draw_filled_circle(bullet->coord.x - camera.x, bullet->coord.y - camera.y, scale, al_map_rgb(255, 255, 0));
    if (bullet->image != NULL) {
        al_draw_bitmap(bullet->image, bullet->coord.x - camera.x - 16, bullet->coord.y - camera.y - 16, 0);
    }
    else {
        game_log("failed to load bullet image");
    }
}

void destroy_bullet(Bullet * bullet){
    // In case you are using image asset for the bullet, free it here
    if (bullet != NULL)
        return;
        //al_destroy_bitmap(bullet->image);
}

/*
    LINKED LIST IMPLEMENTATION FOR BULLET
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF BULLET
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE BULLET
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE BULLET
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

BulletNode * createBulletList(void){
    BulletNode * dummyhead = (BulletNode *) malloc(sizeof(BulletNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertBulletList(BulletNode * dummyhead, Bullet bullet){
    BulletNode * tmp = (BulletNode *) malloc(sizeof(BulletNode));
    tmp->bullet = bullet;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

void updateBulletList(BulletNode * dummyhead, enemyNode * enemyList, Map * map){
    BulletNode * cur = dummyhead->next;
    BulletNode * prev = dummyhead;
    
    while(cur != NULL){
        bool shouldDelete = update_bullet(&cur->bullet, enemyList, map);
        if(shouldDelete){
            prev->next = cur->next;
            destroy_bullet(&cur->bullet);
            free(cur);
            cur = prev;
        }
        prev = cur;
        cur = cur->next;
    }
}

void drawBulletList(BulletNode * dummyhead, Point camera){
    BulletNode * cur = dummyhead->next;
    
    while(cur != NULL){
        draw_bullet(&cur->bullet, camera);
        cur = cur->next;
    }
}

void destroyBulletList(BulletNode * dummyhead){

    BulletNode* del = dummyhead;
    dummyhead = dummyhead->next;
    free(del);

    while(dummyhead != NULL){
        BulletNode * del = dummyhead;
        dummyhead = dummyhead->next;

        destroy_bullet(&del->bullet);
        free(del);
    }
}
