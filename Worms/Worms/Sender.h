#pragma once
#include "Observer.h"
#include "Network.h"
#include "Scenario.h"
#include <string>

class Sender : public Observer
{
public:
	Sender(Network* network);
	~Sender();
	void update(void* subject, void* tool);
	Packet Packet;
	Network* network;

};

