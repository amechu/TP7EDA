#include "EventGenerator.h"
#include "GameSettings.h"

#define WALKDURATION 1000
#define TIMEFIX 200
using namespace std;
EventGenerator::EventGenerator()
{
}


EventGenerator::~EventGenerator()
{
}

void EventGenerator::pushEvent(Event event)
{
	eventQueue.push(event);
}

Event EventGenerator::fetchEvent()
{
	Event returnVal = eventQueue.front();
	eventQueue.pop();
	return returnVal;
}

Event EventGenerator::transformAllegroEvent(AllegroTools * allegroTools)
{
	ALLEGRO_EVENT allegroEvent;
	Event Event = { NOEVENT , 0 };

	if (al_get_next_event(allegroTools->Queue, &allegroEvent)) {

		if (allegroEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			Event.type = QUITLOCAL;
		}

		else if (allegroEvent.type == ALLEGRO_EVENT_TIMER) {
			Timer.stop();
			if (activeTimer[gameSettings::Left] == true && Timer.getTime() >= 100) {
				if (Timer.getTime() >= (firstMove ? WALKDURATION + TIMEFIX : WALKDURATION)) {
					Timer.start();
					sentMove = false;
					firstMove = false;
				}
				else {
					Event.type = REFRESH;
				}
				if (!sentMove) {
					Event.type = REFRESHLEFT;
					sentMove = true;
				}
				else {
					Event.type = REFRESH;
				}
			}
			else if (activeTimer[gameSettings::Right] == true && Timer.getTime() >= 100) {
				if (Timer.getTime() >= (firstMove ? WALKDURATION + TIMEFIX : WALKDURATION)) {
					Timer.start();
					sentMove = false;
					firstMove = false;
				}
				else {
					Event.type = REFRESH;
				}
				if (!sentMove) {
					Event.type = REFRESHRIGHT;
					sentMove = true;
				}
				else {
					Event.type = REFRESH;
				}
			}
			else {
				if (activeTimer[gameSettings::Jump] == true && Timer.getTime() >= 640) {
					activeTimer[gameSettings::Jump] = false;
				}
				Event.type = REFRESH;
			}
		}

		else if (allegroEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Left]) {
				if (activeTimer[gameSettings::Left] == false && activeTimer[gameSettings::Right] == false && activeTimer[gameSettings::Jump] == false) {
					activeTimer[gameSettings::Left] = true;
					Timer.start();
				}
			}
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Right]) {
				if (activeTimer[gameSettings::Left] == false && activeTimer[gameSettings::Right] == false && activeTimer[gameSettings::Jump] == false) {
					activeTimer[gameSettings::Right] = true;
					Timer.start();
				}
			}
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Jump]) {
				if (activeTimer[gameSettings::Left] == false && activeTimer[gameSettings::Right] == false) {
					activeTimer[gameSettings::Jump] = true;
					Event.type = JUMP;
					Timer.start();
				}
			}
		}

		else if (allegroEvent.type == ALLEGRO_EVENT_KEY_UP) {
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Left]) {
				if (activeTimer[gameSettings::Left] == true) {
					if (Timer.getTime() < 100) {
						Event.type = TOGGLELEFT;
					}
				}
				activeTimer[gameSettings::Left] = false;
				sentMove = false;
				firstMove = true;
			}
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Right]) {
				if (activeTimer[gameSettings::Right] == true) {
					if (Timer.getTime() < 100) {
						Event.type = TOGGLERIGHT;
					}
				}
				activeTimer[gameSettings::Right] = false;
				sentMove = false;
				firstMove = true;
			}
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Jump]) {
				//Nothing to do here
			}
		}
	}
	else {
		Event.type = NOEVENT;
	}

	return Event;
}

Event EventGenerator::transformNetworkEvent(Network* Network)
{
	Event Event = { NOEVENT, 1 }; // lo puse en mausculas pq hace lio con otro event de windows
	Packet Packet;
	Packet = Network->fetchRecieved();

	if (Packet.header == MOVE_)
	{
		if (Packet.action == ACTIONLEFT)
		{
			Event.type = LEFT;
		}
		else if (Packet.action == ACTIONRIGHT)
		{
			Event.type = RIGHT;
		}
		else if (Packet.action == ACTIONJUMP)
		{
			Event.type = JUMP;
		}
		else if (Packet.action == ACTIONTOGGLE)
		{
			Event.type = TOGGLE;
		}
	}
	else if ((Packet.header == ACK_) && (Packet.id == 0)) // caso del primer i'm ready
	{
		if (!otherWormCreated) {
			Event.type = NEWWORM; //ME LLEGO EL ACK QUE ENTENDIO CREO EL WORM
			Event.id = Network->getOtherWormPos();
			otherWormCreated = true;
		}
	}
	else if ((Packet.header == ERROR_))
	{
		Event.type = QUIT;
	}
	else if ((Packet.header == QUIT_))
	{
		Event.type = QUIT;
	}
	else if ((Packet.header == IAMRDY)) // caso del primer i'm ready
	{
		if (!otherWormCreated) {
			otherWormCreated = true;
			Event.type = NEWWORM; //ME LLEGO EL ACK QUE ENTENDIO CREO EL WORM
			Event.id = Network->getOtherWormPos();
		}
	}
	else
	{
		Event.type = NOEVENT;
	}
	return Event;
}

void EventGenerator::checkIncomingEvents(AllegroTools * allegroTools, Network * Network)
{
	Event ev;

	ev = transformAllegroEvent(allegroTools);
	if (ev.type != NOEVENT) {
		pushEvent(ev);
	}
	else {
		al_flush_event_queue(allegroTools->Queue);
	}
	ev = transformNetworkEvent(Network);
	if (ev.type != NOEVENT) {
		pushEvent(ev);
	}
	else {
		al_flush_event_queue(allegroTools->Queue);
	}
}

