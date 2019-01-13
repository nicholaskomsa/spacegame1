#include <string>
#include <vector>
#include <type_traits>
#include <memory>

#pragma once

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}






/*
















class NetworkType {
public:
	virtual ~NetworkType() {}

	//write the type to the data
	virtual int write(char* data) = 0;
	//read the data into the type
	virtual int read(char* data, int maxSize) = 0;

	virtual int size() = 0;
};
class NetworkType_Int32 : public NetworkType {
	__int32 mData;
public:
	virtual ~NetworkType_Int32() {}

	int size() { return sizeof(__int32); }

	int write(char* data) {
		memcpy(data, &mData, size() );
		return size();
	}
	int read(char* data, int maxSize ) {
		if (size() > maxSize) return 0;

		memcpy(&mData, data, size());
		return size();
	}
};


class NetworkObjectID {
public:
	enum class NetworkIDs : __int32 { NONE, TEST1 };
public:
	NetworkIDs mID;

	NetworkObjectID(NetworkIDs id)
		:mID(id)
	{}
	NetworkType_Int32 toNetworkType(){
		
		return NetworkType_Int32( to_underlying(mID) );
	}
};

class NetworkObject {
public:
	virtual void encode(NetworkData& data)=0;
	virtual void decode(NetworkData& data)=0;
};


class NetworkData {

	std::vector<char> mData;
	std::size_t mChunkSize;
	std::size_t mOffset;
public:

	NetworkData& operator>>(NetworkType& type) {
		//write to type
		

		if( type.size() + mData.size())
		int written = type.write(&data[offset]);
	}
	NetworkData& operator<<(NetworkData& type) {
		int read = type.read(&data[offset]);
	}

	void clear() {
		length = 0;
		data.clear();
	}
};



class NetworkObject_test1 :	public NetworkObject {
	
	NetworkType_Int32 mI;
	NetworkType_Float mF;

public:

	void encode( NetworkData& data ) {
		data << NetworkObjectID( NetworkObjectID::NetworkIDs::TEST1 ).toNetworkType() << mI << mF;
	}
	
	void decode(NetworkData& data) {
		//if we get to this point then objectID was alraedy removed
		data >> mI >> mF;
	}
};






class NetworkDataManager {


	std::vector< std::shared_ptr<NetworkType> > mDatas;
	std::size_t mDatasLength{ 0 };

	NetworkData mCompiledData;

public:
	NetworkDataID getIdentifier() {

	}

	NetworkDataManager& operator<<(NetworkObject& obj) {



		obj.encode(mData);



		return *this;
	}
	NetworkDataManager& operator>>(NetworkObject& obj) {
		obj.decode();

		return *this;
	}

	void clear() {
		mData.clear();
	}
};

int main() {

	GameObjectTest test;
	GameObjectTest test2;
	GameObjectTest2 test3;

	NetworkData data;

	NetworkDataManager dataOutput;

	dataOutput << test1 << test2 << test3;

	NetworkDataManager input;

	NetworkDataID id = input.getIdentifier();

	switch (id) {
	case NetworkDataID::NetworkDataIDs::TESTOBJECT1:
		
		input >> test1;
		
		break;
	case GameObjectTest2::ID:

		input >> test2;

		break;
	}





}



















































class NetworkData {
public:
	std::string str;
	std::size_t length;

};


class NetworkDataMessage {
public:
	std::string msg;
	std::size_t size{ 0 };
	std::size_t offset{ 0 };

};





class NetworkInt {
	__int32 mI;
public:
	NetworkInt(__int32 i)
		:mI(i) {}

	NetworkInt(NetworkData data) {
		mI = *reinterpret_cast<int*>(const_cast<char*>(data.str.data()));
	}

	int size() { return sizeof(mI); }
	__int32 data() { return mI; }

	operator NetworkData() {
		NetworkData data;
		data.length = size();
		data.str.resize(data.length);
		memcpy(const_cast<char*>(data.str.data()), &mI, data.length);
		return data;
	}

	void get(NetworkDataMessage& msg ) {
		mI = *reinterpret_cast<int*>(const_cast<char*>(msg.msg.data()));
		msg.offset += size();
	}
};


class NetworkDataID {
public:
	enum class NetworkDataIDs : int { NONE, TESTOBJECT1 };
	NetworkDataIDs id;

	NetworkDataID(NetworkDataIDs id1)
		:id(id1) {}

	operator NetworkData() {
		NetworkData data;
		data.length = sizeof(id);
		data.str.resize(data.length);
		memcpy(const_cast<char*>(data.str.data()), &id, data.length);
		return data;
	}

	NetworkDataIDs get(NetworkDataMessage& msg) {

		//first get the id of the message
		NetworkInt msgID(0); 
		msgID.get(msg);
		
		NetworkDataIDs id = to_underlying(msgID.data());
		return id;
	}
};


class NetworkDataMessageOut : public NetworkDataMessage {


	std::vector<NetworkData> mNetworkDatas;


	void setFirstMessageToMessageCount() {
		NetworkInt size(mNetworkDatas.size());
		NetworkData& first = mNetworkDatas[0];
		first = size;
	}

public:

	void init() {
		NetworkData firstMsg;
		firstMsg.length = 0;
		firstMsg.str = "";
		mNetworkDatas.push_back(firstMsg);
	}

	void push(NetworkData& data) {
		mNetworkDatas.push_back(data);
		mMessageSize += data.length;
	}

	void compileMessage() {
		mMessage.resize(mMessageSize);

		setFirstMessageToMessageCount();

		char* offset = const_cast<char*>(mMessage.data());
		for (std::size_t d = 0; d < mNetworkDatas.size(); d++) {
			memcpy(offset, const_cast<char*>(mNetworkDatas[d].str.data()), mNetworkDatas[d].length);
			offset += mNetworkDatas[d].length;
		}

		mNetworkDatas.clear();
	}
};




class TestGameObject {

	NetworkInt x, y;


public:

	void serialize(NetworkDataMessageOut& message) {

		NetworkDataID objId(NetworkDataID::NetworkDataIDs::TESTOBJECT1);

		NetworkData id = objId;
		message.push(id);

		NetworkData sx = x, sy = y;
		message.push(sx);
		message.push(sy);
	}

};









class GameNetwork {

protected:


public:



	void test() {

		TestGameObject testObj;

		NetworkDataMessage networkMsg;
		testObj.serialize(networkMsg);

		networkMsg.compileMessage();

		RakNet::Packet* incomingPacket;
		NetworkDataMessage networkMessageIncoming(incomingPacket);

		networkMessageIncoming.decompileMessage();



	}



};



*/