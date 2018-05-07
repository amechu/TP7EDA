#include "Packet.h"



Packet::Packet()
{
}


Packet::~Packet()
{
}
using namespace gameSettings;
std::string Packet::makePacket(uint8_t header, uint16_t action, uint32_t id, uint16_t pos)
{
	std::string string;

	switch (header) {
	case QUIT_:
		string = QUIT_;
		this->header = QUIT_;
		break;
	case ERROR_:
		string = ERROR_;
		this->header = ERROR_;
		break;
	case IAMRDY:
		string = IAMRDY;
		this->header = IAMRDY;
		string += (posToBigEndian(&pos));
		this->pos = pos;
		break;
	case ACK_:
		string = ACK_;
		this->header = ACK_;
		string += (idToBigEndian(&id));
		this->id = id;
		break;
	case MOVE_:
		string = MOVE_;
		this->header = MOVE_;
		string += action;
		this->action = action;
		string += (idToBigEndian(&id));
		this->id = id;
		break;
	}


	return string;
}

std::string Packet::posToBigEndian(uint16_t* pos)
{
	int8_t* p2num = (int8_t*)pos;
	std::string retValue;
	int8_t array[2] = { *(p2num + 1), *(p2num) };

	for (int i = 0; i < 2; i++)
		retValue += array[i];

	return retValue;
}

std::string Packet::idToBigEndian(uint32_t* id)
{
	int8_t * p2num = (int8_t *)id;
	std::string retValue;
	int8_t array[4] = { *(id + 3), *(id + 2), *(id + 1), *id };

	if (*id == 0) {
		retValue[0] = 0;
	}
	else {
		for (int i = 0; i < 4; i++)
			retValue += array[i];
	}

	return retValue;
}
