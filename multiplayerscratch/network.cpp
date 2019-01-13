#include "network.h"

#include <string>
#include <chrono>

#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#include <cefprocess.h>
#include "gamemode.h"
#include "gamemode_splash.h"




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


