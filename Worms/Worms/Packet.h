#pragma once

#include "Worm.h"
#include <stdint.h>
#include <string>
#include "GameSettings.h"
#include <cstdint>


#define ACK_ (0x01)
#define IAMRDY (0x20)
#define MOVE_ (0x30)
#define QUIT_	(0xFF)
#define ERROR_ (0xE0)

#define ACTIONRIGHT ('D')
#define ACTIONLEFT ('L')
#define ACTIONJUMP ('J')
#define ACTIONTOGGLE ('T')

class Packet
{
public:
	Packet();
	~Packet();

	std::string makePacket(uint8_t type, uint16_t action=0, uint32_t id=0, uint16_t pos=0);
	std::string posToBigEndian(uint16_t* pos);
	std::string idToBigEndian(uint32_t* id);
	uint8_t header;
	uint8_t action;
	uint32_t id;
	uint16_t pos;

};

