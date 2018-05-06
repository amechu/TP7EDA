#pragma once
#include "allegro5\allegro.h"
#include "allegro5\allegro_image.h"
#include <string>
#include <array>

class DrawingInfo
{
public:
	DrawingInfo();
	~DrawingInfo();
	void LoadWormImages();
	void arrangeWormCycle();

	std::array<ALLEGRO_BITMAP*, 51> Wormcycle;
	std::array<ALLEGRO_BITMAP*, 15> WormWalk; //Worm walk bitmaps
	std::string str = "wwalk-F";
	std::string ng = ".png";

	std::array<ALLEGRO_BITMAP*, 10> WormJump; //Worm jump bitmaps
	std::string STR = "wjump-F";
	std::string NG = ".png";

	ALLEGRO_BITMAP *Background;
	ALLEGRO_BITMAP *WindowsBackground;
};

