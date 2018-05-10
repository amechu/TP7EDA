#pragma once
#include "AllegroTools.h"
#include "EventHandling.h"
#include "Timer.h"
#include <queue>
#include "GameSettings.h"
#include "Network.h"


class EventGenerator
{
public:
	EventGenerator();
	~EventGenerator();
	void pushEvent(Event event);
	Event fetchEvent();
	Event transformAllegroEvent(AllegroTools* allegroTools);
	Event transformNetworkEvent(Network* Network);
	void checkIncomingEvents(AllegroTools* allegroTools, Network* Network);
	std::queue<Event> eventQueue;


private:
	bool activeTimer[gameSettings::TotalActions] = { 0 }; 
	Timer Timer;
	bool sentMove = false;
	bool firstMove = true;
	bool otherWormCreated = false;
};

