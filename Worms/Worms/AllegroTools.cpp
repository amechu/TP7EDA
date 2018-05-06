#include "AllegroTools.h"
#include "GameSettings.h"
#include <iostream>


AllegroTools::AllegroTools()
{
	this->Timer = NULL;
	this->Queue = NULL;
	this->Display = NULL;
	this->Font = NULL;
}


AllegroTools::~AllegroTools()
{
	al_destroy_display(this->Display);
	al_destroy_timer(this->Timer);
	al_destroy_event_queue(this->Queue);
	al_destroy_font(this->Font);
}

bool AllegroTools::Init() {

	bool ret = false;

	if (al_init()) {
		if (al_init_font_addon()) {
			if (al_init_ttf_addon()) {
				if (al_init_primitives_addon()) {
					if (al_init_image_addon()) {
						if (al_install_keyboard()) {
							if (al_install_mouse()) {
								if (this->Display = al_create_display(gameSettings::SCREEN_W, gameSettings::SCREEN_H)) {
									if (this->Timer = al_create_timer(1 / gameSettings::FPS)) {
										if ((drawingInfo.Background = al_create_bitmap(gameSettings::SCREEN_W, gameSettings::SCREEN_H)) && (drawingInfo.WindowsBackground = al_create_bitmap(gameSettings::SCREEN_W, gameSettings::SCREEN_H))) {
											if (this->Queue = al_create_event_queue()) {
												if ((drawingInfo.Background = al_load_bitmap("Scenario.png")) && (drawingInfo.WindowsBackground = al_load_bitmap("stars.png"))) {
													if (this->Font = al_load_font("montserrat.ttf", -24, 0)) {
														ret = true;
													}
													else {
														ret = false;
														al_destroy_display(this->Display);
														al_destroy_timer(this->Timer);
														al_destroy_bitmap(drawingInfo.Background);
														al_destroy_event_queue(this->Queue);
														al_destroy_bitmap(drawingInfo.WindowsBackground);
													}
												}

												else {
													ret = false;
													al_destroy_display(this->Display);
													al_destroy_timer(this->Timer);
													al_destroy_bitmap(drawingInfo.Background);
													al_destroy_bitmap(drawingInfo.WindowsBackground);
													al_destroy_event_queue(this->Queue);
												}
											}
											else {
												al_destroy_display(this->Display);
												al_destroy_timer(this->Timer);
												al_destroy_bitmap(drawingInfo.Background);
											}
										}
										else {
											ret = false;
											al_destroy_display(this->Display);
											al_destroy_timer(this->Timer);
										}
									}
									else {
										ret = false;
										al_destroy_display(this->Display);
									}
								}
								else
									ret = false;
							}
							else
								ret = false;
						}
						else
							ret = false;
					}
					else
						ret = false;
				}
				else
					ret = false;
			}
			else
				ret = false;
		}
		else
			ret = false;
	}

	for (int i = 0; i < 15; i++) {
		if (drawingInfo.WormWalk[i] = al_create_bitmap(60, 60)) {}
		else ret = false;
	}

	for (int i = 0; i < 10; i++) {
		if (drawingInfo.WormJump[i] = al_create_bitmap(60, 60)) {}
		else ret = false;
	}

	if (ret) {
		al_register_event_source(this->Queue, al_get_display_event_source(this->Display));
		al_register_event_source(this->Queue, al_get_timer_event_source(this->Timer));
		al_register_event_source(this->Queue, al_get_keyboard_event_source());
		al_register_event_source(this->Queue, al_get_mouse_event_source());

		al_start_timer(this->Timer);
	}
	else {
		for (int i = 0; i < 15; i++) {
			al_destroy_bitmap(drawingInfo.WormWalk[i]);
		}
		for (int i = 0; i < 10; i++) {
			al_destroy_bitmap(drawingInfo.WormJump[i]);
		}
	} 
	
	fondo = al_create_bitmap(SCREEN_W, SCREEN_H);
	fondo = al_load_bitmap("counterstrike.jpg");
	buttonHost = al_create_bitmap(bigButtonSizeW, bigButtonSizeH);
	buttonConnect = al_create_bitmap(bigButtonSizeW, bigButtonSizeH);
	buttonQuit = al_create_bitmap(smallButtonSizeW, smallButtonSizeH);
	
	return ret;
}

using namespace gameSettings;
int AllegroTools::askIfHost()
{
	int selection = NONE;
	int button = HOST;
	ALLEGRO_BITMAP* disp = al_get_target_bitmap();
	ALLEGRO_EVENT allegroEvent;
	

	do {
		if (al_get_next_event(Queue, &allegroEvent)) {
			if (allegroEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				selection = QUITTER;
			}
			else if (allegroEvent.type == ALLEGRO_EVENT_TIMER) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_draw_bitmap(fondo, 0, 0, NULL);
				drawBigButton("Create server", 1 * SCREEN_W / 20, (14 * SCREEN_H / 20) + 2 * bigButtonSizeH / 3, buttonHost, "white", button == HOST? true:false);
				drawBigButton("Find server", 1 * SCREEN_W / 20, (16 * SCREEN_H / 20), buttonConnect, "white", button == CLIENT ? true:false);
				drawSmallButton("Quit", 1 * SCREEN_W / 20, (18 * SCREEN_H / 20) - 2 * bigButtonSizeH / 3 - 1, buttonQuit, "white", button == QUITTER? true:false);
				al_set_target_bitmap(disp);
				al_flip_display();
			}
			else if (allegroEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
				if (allegroEvent.keyboard.keycode == ALLEGRO_KEY_ENTER) {
					if (button == HOST)
						selection = HOST;
					else if (button == CLIENT)
						selection = CLIENT;
					else if (button == QUITTER)
						selection = QUITTER;
				}
			}
			else if (allegroEvent.type == ALLEGRO_EVENT_KEY_UP) {
				if (allegroEvent.keyboard.keycode == ALLEGRO_KEY_W || allegroEvent.keyboard.keycode == ALLEGRO_KEY_UP) {
					if (button > HOST)
						button--;
				}
				if (allegroEvent.keyboard.keycode == ALLEGRO_KEY_S || allegroEvent.keyboard.keycode == ALLEGRO_KEY_DOWN) {
					if (button < QUITTER)
						button++;
				}
			}
		}
	} while (selection == NONE);

	al_unregister_event_source(this->Queue, al_get_mouse_event_source());
	al_flush_event_queue(this->Queue);

	return selection;
}

void AllegroTools::drawBigButton(std::string msg, int x, int y, ALLEGRO_BITMAP* bitmap, std::string color, bool selected)
{
	ALLEGRO_BITMAP* disp = al_get_target_bitmap();
	al_set_target_bitmap(bitmap);
	al_clear_to_color(al_color_name("black"));
	al_draw_rectangle(0, 0, bigButtonSizeW, bigButtonSizeH, al_color_name(color.c_str()), outerThickness);
	if (selected)
		al_draw_text(this->Font, al_color_name("orange"), 12, bigButtonSizeH / 2 - fontSize / 2, ALLEGRO_ALIGN_LEFT, msg.c_str());
	else
		al_draw_text(this->Font, al_color_name(color.c_str()), 12, bigButtonSizeH / 2 - fontSize / 2, ALLEGRO_ALIGN_LEFT, msg.c_str());
	al_set_target_bitmap(disp);
	al_draw_bitmap(bitmap, x, y, NULL);
}

void AllegroTools::drawSmallButton(std::string msg, int x, int y, ALLEGRO_BITMAP * bitmap, std::string color, bool selected)
{
	ALLEGRO_BITMAP* disp = al_get_target_bitmap();
	al_set_target_bitmap(bitmap);
	al_clear_to_color(al_color_name("black"));
	al_draw_rectangle(0, 0, smallButtonSizeW, smallButtonSizeH, al_color_name(color.c_str()), outerThickness);
	if (selected)
		al_draw_text(this->Font, al_color_name("orange"), 12, bigButtonSizeH / 2 - fontSize / 2, ALLEGRO_ALIGN_LEFT, msg.c_str());
	else
		al_draw_text(this->Font, al_color_name(color.c_str()), 12, bigButtonSizeH / 2 - fontSize / 2, ALLEGRO_ALIGN_LEFT, msg.c_str());
	al_set_target_bitmap(disp);
	al_draw_bitmap(bitmap, x, y, NULL);
}


