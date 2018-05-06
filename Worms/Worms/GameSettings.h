#pragma once
#include "allegro5\allegro.h"
#include <string>

namespace gameSettings {

	const enum { I_AM_READY, ACK, MOVE, QUITPACKET, ERRORNET };
	const enum { NONE, HOST, CLIENT, QUITTER };
	const enum { Jump, Left, Right, TotalActions, Toggle }; //agrego toggle que indica que debe invertirse el worm
	const int wormKeySet[TotalActions] = { ALLEGRO_KEY_W, ALLEGRO_KEY_A, ALLEGRO_KEY_D }; //Local worm keys (in allegro key format)
	const int GroundLevel = 616;	//Ground level for worms
	const int LeftWall = 685;		//Left boundary for worms
	const int RightWall = 1170;		//Right boundary for worms
	const int WormInitialPosition = LeftWall + 100;
	const double WalkVelocity = 9;
	const double Velocity = 4.5;	//Worm velocity
	const double Gravity = 0.24;	//Worm gravity
	const float FPS = 5.0;			//Frames per second AKA gamespeed
	const float networkTimeLimit = 100;
	const float SCREEN_W = 1920;	//Screen width
	const float SCREEN_H = 696;		//Screen height
	const std::string port = "15667";
}