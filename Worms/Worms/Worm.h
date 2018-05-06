#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "EventHandling.h"

enum class WormState
{
	Iddle, Walking, Jumping
};

enum class WormDirection 
{
	Left, Right
};

typedef struct
{
	double X;
	double Y;
}Point;

class Worm
{
public:
	Worm(unsigned int id, Point position, WormDirection direction);
	~Worm();

	unsigned int getId();
	int getTickCount();
	Point getPosition();
	WormState getState();
	WormDirection getDirection();
	double getPositionX();
	void setDirection(WormDirection dir);
	void setState(WormState state);
	

	void toggle();
	void moveLeft(); //Si la llamada a funcion fue llamada por refresh o por evento &0
	void moveRight();//ya que si la tecla esta presionada cada entrada a esta funcion sera por evento
	void jump();						//y para continuar el ciclo de caminata es necesario una invocacion a esta funcion (move)
	void refresh();						//por evento. (Mirar event generator y funciones de move de Scene)

private:
	long unsigned int id;
	int tickCount = 0;
	Point Position;
	WormState State = WormState::Iddle;
	WormDirection Direction = WormDirection::Left;
};

