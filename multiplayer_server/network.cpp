#include "network.h"


const std::string Network::mDefaultIncomingPW = "nick";
const unsigned short Network::mDefaultServerPort = 1234U;
const unsigned short Network::mDefaultClientPort = 1235U;


unsigned char  Network::GetPacketIdentifier(RakNet::Packet *p) {

	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {

		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}


