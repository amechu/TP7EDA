#pragma once
#include "DrawingInfo.h"
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_color.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <string>
#include "gameSettings.h"
using namespace gameSettings;
class AllegroTools
{
public:
	AllegroTools();
	~AllegroTools();

	bool Init();
	int askIfHost();
	void drawBigButton(std::string msg, int x, int y, ALLEGRO_BITMAP * bitmap, std::string color, bool selected);
	void drawSmallButton(std::string msg, int x, int y, ALLEGRO_BITMAP * bitmap, std::string color, bool selected);

	DrawingInfo drawingInfo;
	ALLEGRO_TIMER * Timer;
	ALLEGRO_EVENT_QUEUE* Queue;
	ALLEGRO_DISPLAY* Display;
	ALLEGRO_FONT* Font;
	ALLEGRO_BITMAP* buttonHost;
	ALLEGRO_BITMAP* buttonConnect;
	ALLEGRO_BITMAP* buttonQuit;
	ALLEGRO_BITMAP* fondo;

	const float fontSize = 24;
	const float outerThickness = 5;
	const float bigButtonSizeW = (3 * SCREEN_W / 20 + 6);
	const float bigButtonSizeH = (1 * SCREEN_H / 20 + 6);
	const float smallButtonSizeW = (2 * SCREEN_W / 20 + 6);
	const float smallButtonSizeH = (1 * SCREEN_H / 20 + 6);
};

