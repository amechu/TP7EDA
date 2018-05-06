#include "Worm.h"
#include "GameSettings.h"

Worm::Worm(unsigned int id, Point position, WormDirection direction)
{
	this->id = id;
	this->Position = position;
	this->Direction = direction;
}


Worm::~Worm()
{
}

unsigned int Worm::getId()
{
	return this->id;
}

int Worm::getTickCount()
{
	return tickCount;
}

Point Worm::getPosition()
{
	return (this->Position);
}

WormState Worm::getState()
{
	return State;
}

WormDirection Worm::getDirection()
{
	return Direction;
}

void Worm::setDirection(WormDirection dir)
{
	this->Direction = dir;
}

void Worm::setState(WormState state)
{
	this->State = state;
}

void Worm::toggle()
{
	if (this->State == WormState::Iddle) {
		if (this->Direction == WormDirection::Left) //Para mas tarde: pensar como hacer esto en una linea
			this->Direction = WormDirection::Right;
		else
			this->Direction = WormDirection::Left;
	}
}

void Worm::moveLeft()
{
	this->State = WormState::Walking;
	this->Direction = WormDirection::Left;

	this->tickCount++;

	if (this->Position.X >= gameSettings::LeftWall) {
		if (tickCount == 22 || tickCount == 22 + 14 || tickCount == 22 + 14 + 14) {
			this->Position.X -= gameSettings::WalkVelocity;
		}
	}

	if (this->tickCount > 50) {
		tickCount = 5;
		this->State = WormState::Iddle;
	}
	
}

void Worm::moveRight()
{
	this->State = WormState::Walking;
	this->Direction = WormDirection::Right;

	this->tickCount++;

	if (this->Position.X <= gameSettings::RightWall) {
		if (tickCount == 22 || tickCount == 22+14 || tickCount == 22+14+14) {
			this->Position.X += gameSettings::WalkVelocity;
		}
	}

	if (this->tickCount > 50) {
		tickCount = 5;
		this->State = WormState::Iddle;
	}

}

void Worm::jump()
{

	double Linear;
	double Cuadratic;

	if (this->State == WormState::Iddle || this->State == WormState::Jumping) {

		if (this->State == WormState::Iddle)
			this->State = WormState::Jumping;

		this->tickCount++;
		if (this->Direction == WormDirection::Left) {

			if (this->tickCount <= 32) {
				if (Position.X > gameSettings::LeftWall + (gameSettings::Velocity * cos(M_PI / 3))) {
					this->Position.X -= (gameSettings::Velocity * cos(M_PI / 3));
				}
				Linear = gameSettings::Velocity * sin(M_PI / 3) * tickCount;
				Cuadratic = (((gameSettings::Gravity) / 2) * tickCount * tickCount);
				this->Position.Y = gameSettings::GroundLevel - Linear + Cuadratic;
			}
			else {
				this->tickCount = 0;
				this->State = WormState::Iddle;
			}
		}
		else if (this->Direction == WormDirection::Right) {

			if (this->tickCount <= 32) {
				if (Position.X < gameSettings::RightWall - (gameSettings::Velocity * cos(M_PI / 3))) {
					this->Position.X += (gameSettings::Velocity * cos(M_PI / 3));
				}
				Linear = gameSettings::Velocity * sin(M_PI / 3) * tickCount;
				Cuadratic = (((gameSettings::Gravity) / 2) * tickCount * tickCount);
				this->Position.Y = gameSettings::GroundLevel - Linear + Cuadratic;
			}
			else {
				this->tickCount = 0;
				this->State = WormState::Iddle;
			}
		}
	}
}

void Worm::refresh()
{
	switch (this->State) {
		case WormState::Iddle: {
			break;
		}
		case WormState::Jumping: {
			jump();
			break;
		}
		case WormState::Walking: {
			switch (this->Direction) {
				case WormDirection::Left: {
					moveLeft();
					break;
				}
				case WormDirection::Right: {
					moveRight();
					break;
				}
			}
		}
	}
}


double Worm::getPositionX()
{
	return Position.X;
}