#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
Button sliderButton;
float setting_volume;
extern bool first_acvatied;



static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    sliderButton = button_create(SCREEN_W / 2 - 150, SCREEN_H / 2 - 7, 300, 20, "Assets/slider.png", "Assets/slider_hovered.png");
    sliderButton.default_img = al_load_bitmap("Assets/slider.png");
    sliderButton.hovered_img = al_load_bitmap("Assets/slider_hovered.png");

    if (first_acvatied) {
        sliderButton.value = 0.3;
		setting_volume = 0.3;
        first_acvatied = false;
    }
	else {
		sliderButton.value = setting_volume;
	}

    sliderButton.hovered = false;

    backButton.CD = 30;
    
}

static void update(void) {

    update_button(&backButton);

	if (mouseState.buttons && backButton.hovered == true && backButton.CD <= 0) {
		setting_volume = sliderButton.value;
        change_scene(create_menu_scene());
    }

    if (mouseState.x >= sliderButton.x && mouseState.x <= sliderButton.x + sliderButton.w &&
        mouseState.y >= sliderButton.y && mouseState.y <= sliderButton.y + sliderButton.h) {
        sliderButton.hovered = true;
        if (mouseState.buttons) {
            sliderButton.value = (float)(mouseState.x - sliderButton.x) / sliderButton.w;
            al_set_mixer_gain(al_get_default_mixer(), sliderButton.value);
        }
    }
    else {
        sliderButton.hovered = false;
    }

    if (backButton.CD > 0)
        backButton.CD--;

}

static void draw(void) {
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 - 180,
        sliderButton.y - 7,
        ALLEGRO_ALIGN_RIGHT,
        "Volume:"
    );

    draw_button(sliderButton);
	al_draw_filled_rectangle(sliderButton.x, sliderButton.y, sliderButton.x + sliderButton.w * sliderButton.value, sliderButton.y + sliderButton.h, al_map_rgb(255, 255, 255));
	al_draw_rounded_rectangle(sliderButton.x, sliderButton.y, sliderButton.x + sliderButton.w, sliderButton.y + sliderButton.h, 2, 2, al_map_rgb(58, 58, 58), 3);

	al_draw_filled_rectangle(sliderButton.x + sliderButton.w * sliderButton.value - 5, sliderButton.y - 5, sliderButton.x + sliderButton.w * sliderButton.value + 5, sliderButton.y + sliderButton.h + 5, al_map_rgb(177, 177, 177));
	al_draw_rounded_rectangle(sliderButton.x + sliderButton.w * sliderButton.value - 5, sliderButton.y - 5, sliderButton.x + sliderButton.w * sliderButton.value + 5, sliderButton.y + sliderButton.h + 5, 2, 2, al_map_rgb(58, 58, 58), 3);

    char volume[10];
    sprintf_s(volume, sizeof(volume), "%02d", (int)(100 * sliderButton.value));

    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 + 180,
        sliderButton.y - 7,
        ALLEGRO_ALIGN_LEFT,
        volume
    );

}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
