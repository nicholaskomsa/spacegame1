#pragma once

#include <functional>
#include <vector>
#include <string>
#include <chrono>

#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

#include <log.h>



#if LIBCAT_SECURITY==1
#include "SecureHandshake.h" // Include header for secure handshake
#endif



class Network {
public:

	typedef std::function<void(RakNet::Packet*)> Callback;

protected:

	RakNet::RakPeerInterface* mPeer{ nullptr };
	RakNet::RakNetStatistics* mRss{ nullptr };

	unsigned short mMaxConnections{ 2 };
	unsigned short mPort{ 1234 };


	Callback mOnDataReceived{ nullptr };


	bool mQuit{ false };


	// Copied from Multiplayer.cpp
	// If the first byte is ID_TIMESTAMP, then we want the 5th byte
	// Otherwise we want the 1st byte
	unsigned char GetPacketIdentifier(RakNet::Packet *p);

public:

	template<typename OBJECT>
	void setOnDataReceived( OBJECT* object, void (OBJECT::*callback)(RakNet::Packet*)) {
		mOnDataReceived = std::bind(callback, object, std::placeholders::_1);
	}

	static std::string compileStringFromData(unsigned char* data, unsigned int length) {
		std::string ret;
		ret.reserve(length);
		for (std::size_t i = 0; i < length; ++i) {
			ret.push_back(data[i]);
		}
		return ret;
	}


	virtual void sendMessageToClients(std::string message) = 0;

	void shutdown(RakNet::TimeMS timeout = 300) {

		mPeer->Shutdown(timeout);
		RakNet::RakPeerInterface::DestroyInstance(mPeer);
	}

	virtual std::string processMessages() = 0;


};

class NetworkServer:	public Network {
public:

	typedef std::vector<RakNet::SystemAddress> ConnectionList;

protected:


	RakNet::TimeMS mServerTimeout{ 30000 };
	RakNet::TimeMS mUnreliableTimeout{ 1000 };

public:

	NetworkServer() {
		LogManager::add("network server", "network_server.log", std::ios::out);
	}
	virtual ~NetworkServer() {}

	void startup( unsigned short maxConnections, std::string incomingPW = "nick", RakNet::TimeMS timeout= 30'000, unsigned short port= 1234   ) {

		LogManager::get("network server") << "Server::startup" << Log::end;

		mMaxConnections = maxConnections;
		mPort = port;

		mPeer = RakNet::RakPeerInterface::GetInstance();

		mPeer->SetIncomingPassword(incomingPW.c_str(), incomingPW.size());
		mPeer->SetTimeoutTime( timeout, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		//	RakNet::PacketLogger packetLogger;
		//	server->AttachPlugin(&packetLogger);

		// Starting the server is very simple.  2 players allowed.
		// 0 means we don't care about a connectionValidationInteger, and false
		// for low priority threads
		// I am creating two socketDesciptors, to create two sockets. One using IPV6 and the other IPV4

		RakNet::SocketDescriptor socketDescriptors[2];
		socketDescriptors[0].port = mPort;
		socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
		socketDescriptors[1].port = mPort;
		socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6
		bool b = mPeer->Startup(mMaxConnections, socketDescriptors, 2) == RakNet::RAKNET_STARTED;
		mPeer->SetMaximumIncomingConnections(mMaxConnections);
		if (!b) {
			LogManager::get("network server") << "Server::setup, failed to start ipv6 ipv4 server" << Log::end;

			// Try again, but leave out IPV6
			b = mPeer->Startup(mMaxConnections, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
			if (!b) {
				LogManager::get("network server") << "Server::setup, failed to start ipv4 server" << Log::end;
			}
			else
				LogManager::get("network server" ) << "Server::setup, started ipv4 server" << Log::end;
		}
		else
			LogManager::get("network server" ) << "Server::setup, started ipv6 ipv4 server" << Log::end;

		mPeer->SetOccasionalPing(true);
		mPeer->SetUnreliableTimeout(mUnreliableTimeout);
	}



	void quit() {
		mQuit = true;
	}
	void stat() {
		int systemAddressIndex = 0;

		mRss = mPeer->GetStatistics(mPeer->GetSystemAddressFromIndex(systemAddressIndex));

		char cmessage[2048];
		StatisticsToString(mRss, cmessage, 2);

		LogManager::get("network server") << cmessage << "\n " << " Ping: " << mPeer->GetAveragePing(mPeer->GetSystemAddressFromIndex(systemAddressIndex)) << Log::end;
	}
	void ping( RakNet::SystemAddress clientToPing = RakNet::UNASSIGNED_SYSTEM_ADDRESS) {
		mPeer->Ping(clientToPing);
	}

	void pingIP( std::string ip ) {
		mPeer->Ping( ip.c_str(), mPort, false);
	}

	void kick(RakNet::SystemAddress clientToKick = RakNet::UNASSIGNED_SYSTEM_ADDRESS) {
		mPeer->CloseConnection(clientToKick, true, 0);
	}

	ConnectionList&& getConnectionList() {
		
		ConnectionList connectionList;

		RakNet::SystemAddress* systems = new RakNet::SystemAddress[mMaxConnections];
		
		mPeer->GetConnectionList( systems, &mMaxConnections );
		for (int i = 0; i < mMaxConnections; i++) {

			connectionList.push_back(systems[i]);
		}

		delete[] systems;
		systems = nullptr;

		return std::move(connectionList);
	}

	void ban(std::string ip) {

		mPeer->AddToBanList(ip.c_str());
	}


	std::string processMessages() {


		for ( RakNet::Packet* p = mPeer->Receive(); p; mPeer->DeallocatePacket(p), p = mPeer->Receive())
		{
			// We got a packet, get the identifier with our handy function
			unsigned char packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier) {

			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				LogManager::get("network server") << "ID_DISCONNECTION_NOTIFICATION from: " <<  p->systemAddress.ToString(true) << Log::end;

				continue;

			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				LogManager::get("network server" ) << "ID_NEW_INCOMING_CONNECTION from: " << p->systemAddress.ToString(true) << ", GUID: " << p->guid.ToString() << Log::end;

				/*
				printf("Remote internal IDs:\n");
				for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
				{
					RakNet::SystemAddress internalId = server->GetInternalID(p->systemAddress, index);
					if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
					{
						printf("%i. %s\n", index + 1, internalId.ToString(true));
					}
				}
				*/
				continue;

			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				LogManager::get("network server" )  << "ID_INCOMPATIBLE_PROTOCOL_VERSION" << Log::end;
				continue;

			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				LogManager::get("network server" ) << "Ping from: " << p->systemAddress.ToString(true) << Log::end;
				continue;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				LogManager::get("network server") << "ID_CONNECTION_LOST from: " <<  p->systemAddress.ToString(true) << Log::end;

				continue;

			default:
				
				if( mOnDataReceived ) mOnDataReceived(p);

				//std::string data = compileStringFromData(p->data, p->length);

				mPeer->Send(reinterpret_cast<const char*>(p->data), *reinterpret_cast<const int*>(&p->length), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);


				LogManager::get("network server") << "processMessages: received a data message" << Log::end;
				break;
			}

		}
		return "";
	}
	void sendMessageToClients(std::string message) {

		mPeer->SendLoopback(message.data(), message.size() );
	}
};


class NetworkClient : public Network {
protected:

	unsigned short mServerPort = 1234;



public:

	NetworkClient() {

		LogManager::add("network client", "network_client.log", std::ios::out);
	}
	virtual ~NetworkClient() {}

	bool connect( std::string ip, std::string password="nick", unsigned short maxConnections=1, unsigned short serverPort= 1234, unsigned short clientPort = 1235) {

		mMaxConnections = maxConnections;
		mPort = clientPort;
		mServerPort = serverPort;

		mPeer = RakNet::RakPeerInterface::GetInstance();

		mPeer->AllowConnectionResponseIPMigration(false);

		RakNet::SocketDescriptor socketDescriptor(mPort, nullptr);
		socketDescriptor.socketFamily = AF_INET;
		mPeer->Startup( mMaxConnections, &socketDescriptor, 1);
		mPeer->SetOccasionalPing(true);

		RakNet::ConnectionAttemptResult car = mPeer->Connect(ip.c_str(), mServerPort, password.c_str(), password.size() );
	
		/*
		unsigned int i;
		for (i = 0; i < client->GetNumberOfAddresses(); i++)
		{
			printf("%i. %s\n", i + 1, client->GetLocalIP(i));
		}
		*/


		bool b = mPeer->Connect(ip.c_str(), mServerPort, password.c_str(), password.size() ) == RakNet::CONNECTION_ATTEMPT_STARTED;

		if (b)
			LogManager::get("network client") << "connect: attempting connection" << Log::end;
		else{
			LogManager::get("network client") << "connect: bad connection attempt" << Log::end;
			return false;
		}
		return true;
	}

	std::string getGUID() {
		return mPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString();
	}


	std::string processMessages() {
		// Get a packet from either the server or the client
	

		for (RakNet::Packet* p = mPeer->Receive(); p; mPeer->DeallocatePacket(p), p = mPeer->Receive())
		{
			// We got a packet, get the identifier with our handy function
			unsigned char packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				LogManager::get("network client") << "processMessages: disconnected" << Log::end;
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				LogManager::get("network client") << "processMessages: already connected; " << p->guid.ToString() << Log::end;
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				LogManager::get("network client") << "processMessages: ID_INCOMPATIBLE_PROTOCOL_VERSION" << Log::end;
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				LogManager::get("network client") << "processMessages: ID_REMOTE_DISCONNECTION_NOTIFICATION" << Log::end;
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				LogManager::get("network client") << "processMessages: ID_REMOTE_CONNECTION_LOST" << Log::end;
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				LogManager::get("network client") << "processMessages: ID_REMOTE_NEW_INCOMING_CONNECTION" << Log::end;
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				LogManager::get("network client") << "processMessages: we were banned from server" << Log::end;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				LogManager::get("network client") << "processMessages: connection attempt failed" << Log::end;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				LogManager::get("network client") << "processMessages: ID_NO_FREE_INCOMING_CONNECTIONS" << Log::end;
				break;

			case ID_INVALID_PASSWORD:
				LogManager::get("network client") << "processMessages: ID_INVALID_PASSWORD" << Log::end;
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				LogManager::get("network client") << "processMessages: ID_CONNECTION_LOST" << Log::end;
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				LogManager::get("network client") << "processMessages: ID_CONNECTION_REQUEST_ACCEPTED; " << p->systemAddress.ToString(true) << ", " << p->guid.ToString() << Log::end;
				LogManager::get("network client") << "processMessages: external address: " << mPeer->GetExternalID(p->systemAddress).ToString(true) << Log::end;
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				LogManager::get("network client") << "processMessages: Ping from " << p->systemAddress.ToString(true) << Log::end;
				break;
			default:

				if (mOnDataReceived) mOnDataReceived(p);

				LogManager::get("network client") << "processMessages: received a data message" << Log::end;
				break;
			}
		}
		return "";
	}
	void sendMessageToClients(std::string message) {

		mPeer->Send(message.data(), message.size(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}
};

