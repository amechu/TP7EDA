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
		break;
	}
	case TOGGLE: {
		Scene->Toggle(Event, allegroTools, WormDirection::Left); //direction unused
		break;
	}
	case TOGGLELEFT: {
		Scene->Toggle(Event, allegroTools, WormDirection::Left);
		break;
	}
	case TOGGLERIGHT: {
		Scene->Toggle(Event, allegroTools, WormDirection::Right);
		break;
	}
	case RIGHT: {
		Scene->moveRight(Event, allegroTools);
		break;
	}
	case LEFT: {
		Scene->moveLeft(Event, allegroTools);
		break;
	}
	case REFRESHRIGHT: {
		if (Scene->getWormState(Event) == WormState::Iddle) {
			Scene->setWormState(Event, WormState::Walking);
			Scene->directWorm(Event, WormDirection::Right);
		}
		Scene->setLastAction(REFRESHRIGHT, LOCAL);
		Scene->Refresh(allegroTools);
		break;
	}
	case REFRESHLEFT: {
		if (Scene->getWormState(Event) == WormState::Iddle) {
			Scene->setWormState(Event, WormState::Walking);
			Scene->directWorm(Event, WormDirection::Left);
		}
		Scene->setLastAction(REFRESHLEFT, LOCAL);
		Scene->Refresh(allegroTools);
		break;
	}
	case REFRESH: {
		Scene->setLastAction(REFRESH, LOCAL);
		Scene->Refresh(allegroTools);
		break;
	}
	case QUITLOCAL: {
		Scene->Quit(allegroTools);
		break;
	}
	case NOEVENT: {
		break;
	}
	case NEWWORM: {
		Scene->createNewWorm(1, { (double)Event.id, gameSettings::GroundLevel}, WormDirection::Left);
		break;
	}

	}
}