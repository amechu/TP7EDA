#include "DrawingInfo.h"

using namespace std;

DrawingInfo::DrawingInfo()
{
	for (int i = 0; i < 15; i++)
		this->WormWalk[i] = NULL;
	for (int i = 0; i < 10; i++)
		this->WormJump[i] = NULL;
	this->Background = NULL;
	this->WindowsBackground = NULL;

}

DrawingInfo::~DrawingInfo()
{
	for (int i = 0; i < 15; i++) {
		al_destroy_bitmap(this->WormWalk[i]);
	}

	for (int i = 0; i < 10; i++) {
		al_destroy_bitmap(this->WormJump[i]);
	}

	al_destroy_bitmap(this->Background);
	al_destroy_bitmap(this->WindowsBackground);
}

void DrawingInfo::LoadWormImages() {
	string string;

	for (int i = 1; i < 16; i++) {
		string = str + to_string(i) + ng;    //Se inicializan los bitmaps de los worms
		this->WormWalk[i - 1] = al_load_bitmap(string.c_str());
	}

	for (int i = 1; i < 11; i++) {
		string = STR + to_string(i) + NG;
		this->WormJump[i - 1] = al_load_bitmap(string.c_str());
	}
}

void DrawingInfo::arrangeWormCycle()
{
	//No se usa la posicion 0
	//Confirmacion
	for (int i = 1; i <= 5; i++) {
		this->Wormcycle[i] = this->WormWalk[0];
	}
	//Warmup
	for (int i = 6; i <= 8; i++) {
		this->Wormcycle[i] = this->WormWalk[i-6];
	}
	//Primera fase
	for (int i = 9; i <= 16; i++) {
		this->Wormcycle[i] = this->WormWalk[i-6];
	}
	for (int i = 16; i <= 20; i++) {
		this->Wormcycle[i+1] = this->WormWalk[i-6];
	}
	this->Wormcycle[22] = this->WormWalk[3];
	//Segunda fase
	for (int i = 9; i <= 16; i++) {
		this->Wormcycle[i+1+13] = this->WormWalk[i - 6];
	}
	for (int i = 16; i <= 20; i++) {
		this->Wormcycle[i+1+13+1] = this->WormWalk[i - 6];
	}
	this->Wormcycle[22+1+13] = this->WormWalk[3];
	//Tercera fase
	for (int i = 9; i <= 16; i++) {
		this->Wormcycle[i+1+13+1+13] = this->WormWalk[i - 6];
	}
	for (int i = 16; i <= 20; i++) {
		this->Wormcycle[i+1+13+1+13+1] = this->WormWalk[i - 6];
	}
	this->Wormcycle[22+1+13+1+13] = this->WormWalk[3];
}