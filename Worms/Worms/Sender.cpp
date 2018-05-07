#include "Sender.h"

Sender::Sender(Network* network)
{
	this->network = network;
}


Sender::~Sender()
{
}

void Sender::update(void * subject, void* tool)
{
	Scenario* Scene = (Scenario*)subject;

	if ((Scene->getLastAction()).origin == LOCAL && (Scene->getLastAction().id != REFRESH)) {
		switch ((Scene->getLastAction()).id) {
		case REFRESHLEFT:
			Packet.header = MOVE_;
			Packet.action = ACTIONLEFT;
			Packet.id = (0xFFFFFFFE);
			break;
		case REFRESHRIGHT:
			Packet.header = MOVE_;
			Packet.action = ACTIONRIGHT;
			Packet.id = (0xFFFFFFFE);
			break;
		case JUMP:
			Packet.header = MOVE_;
			Packet.action = ACTIONJUMP;
			Packet.id = (0xFFFFFFFE);
			break;
		case TOGGLELEFT:
		case TOGGLERIGHT:
			Packet.header = MOVE_;
			Packet.action = ACTIONTOGGLE;
			Packet.id = (0xFFFFFFFE);
			break;
		case QUITLOCAL:
			Packet.header = QUIT_;
			Packet.id = 0;
		}
		network->pushToSend(Packet);
	}
}
