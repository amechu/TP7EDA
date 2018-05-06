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

	cout << "TRANSFORM ALLEGRO EVENT" << endl; //DEBUG
	if (al_get_next_event(allegroTools->Queue, &allegroEvent)) {

		if (allegroEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			Event.type = QUITLOCAL;
			cout << "TRANSFORMED QUITLOCAL" << endl; //DEBUG
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
					cout << "TRANSFORM REFRESH" << endl; //DEBUG
				}
				if (!sentMove) {
					Event.type = REFRESHLEFT;
					cout << "TRANSFORM REFRESHLEFT" << endl; //DEBUG
					sentMove = true;
				}
				else {
					Event.type = REFRESH;
					cout << "TRANSFORM REFRESH" << endl; //DEBUG
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
					cout << "REFRESH" << endl; //DEBUG
				}
				if (!sentMove) {
					Event.type = REFRESHRIGHT;
					cout << "TRANSFORM REFRESHRIGHT" << endl; //DEBUG
					sentMove = true;
				}
				else {
					Event.type = REFRESH;
					cout << "TRANSFORM REFRESH" << endl; //DEBUG
				}
			}
			else {
				if (activeTimer[gameSettings::Jump] == true && Timer.getTime() >= 640) {
					activeTimer[gameSettings::Jump] = false;
				}
				Event.type = REFRESH;
				cout << "TRANSFORM REFRESH" << endl; //DEBUG
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
					cout << "TRANSFORM JUMP" << endl; //DEBUG
					Timer.start();
				}
			}
		}

		else if (allegroEvent.type == ALLEGRO_EVENT_KEY_UP) {
			if (allegroEvent.keyboard.keycode == gameSettings::wormKeySet[gameSettings::Left]) {
				if (activeTimer[gameSettings::Left] == true) {
					if (Timer.getTime() < 100) {
						Event.type = TOGGLELEFT;
						cout << "TRANSFORM TOGGLELEFT" << endl; //DEBUG
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
						cout << "TRANSFORM TOGGLERIGHT" << endl; //DEBUG
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
		cout << "TRANSFORM NOEVENT" << endl; //DEBUG
	}

	return Event;
}

Event EventGenerator::transformNetworkEvent(Network* Network)
{
	Event Event = { NOEVENT, 1 }; // lo puse en mausculas pq hace lio con otro event de windows
	Packet Packet;
	Packet = Network->fetchRecieved();

	cout << "TRANSFORMING NETWORK EVENT" << endl; //DEBUG
	if (Packet.header == MOVE_)
	{
		if (Packet.action == ACTIONLEFT)
		{
			Event.type = LEFT;
			cout << "TRANSFORM LEFT" << endl; //DEBUG
		}
		else if (Packet.action == ACTIONRIGHT)
		{
			Event.type = RIGHT;
			cout << "TRANSFORM RIGHT" << endl; //DEBUG
		}
		else if (Packet.action == ACTIONJUMP)
		{
			Event.type = JUMP;
			cout << "TRANSFORM JUMP" << endl; //DEBUG
		}
		else if (Packet.action == ACTIONTOGGLE)
		{
			Event.type = TOGGLE;
			cout << "TRANSFORM TOGGLE" << endl; //DEBUG
		}
	}
	else if ((Packet.header == ACK_) && (Packet.id == 0)) // caso del primer i'm ready
	{
		Event.type = NEWWORM; //ME LLEGO EL ACK QUE ENTENDIO CREO EL WORM
		Event.id = Network->getOtherWormPos();
		cout << "TRANSFORM NEWWORM" << endl; //DEBUG
	}
	else if ((Packet.header == ERROR_))
	{
		Event.type = QUIT;
		cout << "TRANSFORM QUIT" << endl; //DEBUG
	}
	else if ((Packet.header == QUIT_))
	{
		Event.type = QUIT;
		cout << "TRANSFORM QUIT" << endl; //DEBUG
	}
	else if ((Packet.header == IAMRDY)) // caso del primer i'm ready
	{
		Event.type = NEWWORM; //ME LLEGO EL ACK QUE ENTENDIO CREO EL WORM
		Event.id = Network->getOtherWormPos();
		cout << "TRANSFORM NEWWORM" << endl; //DEBUG
	}
	else
	{
		Event.type = NOEVENT;
		cout << "TRANSFORM NO EVENT" << endl; //DEBUG
	}
	return Event;
}

void EventGenerator::checkIncomingEvents(AllegroTools * allegroTools, Network * Network)
{
	pushEvent(transformAllegroEvent(allegroTools));
	pushEvent(transformNetworkEvent(Network));
}

