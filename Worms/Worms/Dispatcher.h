#pragma once
#include "Scenario.h"
#include "EventGenerator.h"
#include "EventHandling.h"

class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher();
	void Dispatch(Event Event, Scenario* Scene, AllegroTools* allegroTools);
};

