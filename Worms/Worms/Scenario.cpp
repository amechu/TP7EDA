#include "Scenario.h"


Scenario::Scenario()
{
}


Scenario::~Scenario()
{
}

void Scenario::createNewWorm(unsigned int id, Point position, WormDirection direction)
{
	Worm* worm = new Worm(id, position, direction);
	this->Worms.push_back(*worm);
}

void Scenario::destroyWorm(unsigned int id)
{
	for (std::vector<Worm>::iterator it = Worms.begin(); it != Worms.end(); it++) {
		if ((*it).getId() == id)
			Worms.erase(it);
	}
}

std::vector<Worm> Scenario::getWormList()
{ 
	return (this->Worms);
}

void Scenario::moveLeft(Event event, AllegroTools* allegroTools)
{
	for (Worm& worm : this->Worms) {
		if (worm.getId() == event.id) {
			worm.moveLeft(); 
		}
	}
	this->lastAction.id = LEFT;
	this->lastAction.origin = (event.id == 0 ? LOCAL : EXTERNAL);
	this->notify(allegroTools);
}

void Scenario::moveRight(Event event, AllegroTools* allegroTools)
{
	for (Worm& worm : this->Worms) {
		if (worm.getId() == event.id) {
			worm.moveRight();
		}
	}
	this->lastAction.id = RIGHT;
	this->lastAction.origin = (event.id == 0 ? LOCAL : EXTERNAL);
	this->notify(allegroTools);
}

void Scenario::Jump(Event event, AllegroTools* allegroTools)
{
	for (Worm& worm : this->Worms) {
		if (worm.getId() == event.id) {
			worm.jump();
		}
	}
	this->lastAction.id = JUMP;
	this->lastAction.origin = (event.id == 0 ? LOCAL : EXTERNAL);
	this->notify(allegroTools);
}

void Scenario::Toggle(Event event, AllegroTools* allegroTools, WormDirection currentdir)
{
	for (Worm& worm : this->Worms) {
		if (worm.getId() == event.id) {
			if (event.id == 0) {
				if (worm.getDirection() == WormDirection::Left && currentdir == WormDirection::Right) {
					worm.setDirection(currentdir);
					this->lastAction.id = TOGGLERIGHT;
					this->lastAction.origin = LOCAL;
					this->notify(allegroTools);
				}
				if (worm.getDirection() == WormDirection::Right && currentdir == WormDirection::Left) {
					worm.setDirection(currentdir);
					this->lastAction.id = TOGGLELEFT;
					this->lastAction.origin = LOCAL;
					this->notify(allegroTools);
				}
			}
			else {
				if (worm.getDirection() == WormDirection::Left) {
					worm.setDirection(WormDirection::Right);
				}
				else {
					worm.setDirection(WormDirection::Left);
				}
				this->lastAction.id = TOGGLE;
				this->lastAction.origin = EXTERNAL;
				this->notify(allegroTools);
			}
		}
	}
}

void Scenario::Refresh(AllegroTools* allegroTools)
{
	for (Worm& worm : this->Worms) {
		worm.refresh();
	}
	this->lastAction.id = REFRESH;
	this->notify(allegroTools);
}

void Scenario::Quit(AllegroTools * allegroTools)
{
	this->lastAction.id = QUITLOCAL;
	this->lastAction.origin = LOCAL;
	this->notify(allegroTools);
}

void Scenario::directWorm(Event event, WormDirection dir)
{
	for (Worm& worm : Worms) {
		if (worm.getId() == event.id) {
			worm.setDirection(dir);
		}
	}
}

void Scenario::setWormState(Event event, WormState state)
{

	for (Worm& worm : Worms) {
		if (worm.getId() == event.id) {
			worm.setState(state);
		}
	}
}

WormState Scenario::getWormState(Event event)
{
	WormState state;

	for (Worm& worm : Worms) {
		if (worm.getId() == event.id) {
			state = worm.getState();
		}
	}

	return state;
}

Worm& Scenario::getWorm(int id)
{
	Worm& worm = Worms.at(id);
	return worm;
}