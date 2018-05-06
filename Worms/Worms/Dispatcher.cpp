#include "Dispatcher.h"
#include "AllegroTools.h"

using namespace std;

Dispatcher::Dispatcher()
{
}


Dispatcher::~Dispatcher()
{
}

void Dispatcher::Dispatch(Event Event, Scenario* Scene, AllegroTools* allegroTools)
{

	switch (Event.type) {
	case JUMP: {
		Scene->Jump(Event, allegroTools);
		cout << "JUMP" << endl; //DEBUG
		break;
	}
	case TOGGLE: {
		Scene->Toggle(Event, allegroTools, WormDirection::Left); //direction unused
		cout << "TOGGLE" << endl; //DEBUG
		break;
	}
	case TOGGLELEFT: {
		Scene->Toggle(Event, allegroTools, WormDirection::Left);
		cout << "TOGGLELEFT" << endl; //DEBUG
		break;
	}
	case TOGGLERIGHT: {
		Scene->Toggle(Event, allegroTools, WormDirection::Right);
		cout << "TOGGLERIGHT" << endl; //DEBUG
		break;
	}
	case RIGHT: {
		Scene->moveRight(Event, allegroTools);
		cout << "RIGHT" << endl; //DEBUG
		break;
	}
	case LEFT: {
		Scene->moveLeft(Event, allegroTools);
		cout << "LEFT" << endl; //DEBUG
		break;
	}
	case REFRESHRIGHT: {
		if (Scene->getWormState(Event) == WormState::Iddle) {
			Scene->setWormState(Event, WormState::Walking);
			Scene->directWorm(Event, WormDirection::Right);
		}
		Scene->Refresh(allegroTools);
		cout << "REFRESHRIGHT" << endl; //DEBUG
		break;
	}
	case REFRESHLEFT: {
		if (Scene->getWormState(Event) == WormState::Iddle) {
			Scene->setWormState(Event, WormState::Walking);
			Scene->directWorm(Event, WormDirection::Left);
		}
		Scene->Refresh(allegroTools);
		cout << "REFRESHLEFT" << endl; //DEBUG
		break;
	}
	case REFRESH: {
		Scene->Refresh(allegroTools);
		cout << "REFRESH" << endl; //DEBUG
		break;
	}
	case QUITLOCAL: {
		Scene->Quit(allegroTools);
		cout << "QUIT" << endl; //DEBUG
		break;
	}
	case NOEVENT: {
		cout << "NOEVENT" << endl; //DEBUG
		break;
	}
	case NEWWORM: {
		Scene->createNewWorm(1, { (double)Event.id, gameSettings::GroundLevel }, WormDirection::Left);
		cout << "NEWWORM" << endl; //DEBUG
		break;
	}

	}
}