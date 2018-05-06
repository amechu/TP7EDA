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
		break;
	case ERROR_:
		string = ERROR_;
		break;
	case IAMRDY:
		string = IAMRDY;
		string += (posToBigEndian(&pos));
		break;
	case ACK_:
		string = ACK_;
		string += (idToBigEndian(&id));
		break;
	case MOVE_:
		string = MOVE_;
		string += action;
		string += (idToBigEndian(&id));
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
