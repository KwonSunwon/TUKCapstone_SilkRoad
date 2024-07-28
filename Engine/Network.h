#pragma once

#include "LockQueue.h"
#include "MonoBehaviour.h"
#include "Packet.h"

static const int MAX_PLAYER = 2;
static const int SERVER_PORT = 9000;
static const int TIMEOUT = 5;

#define SEND_PACKET_PER_SEC 1.f / 30.f

// Packet Type
//enum class PACKET_TYPE {
//	PLAYER,
//	ENEMY,
//	ITEM,
//	// ...
//	NET,
//	END
//};

// Packet Struct
//struct PacketHeader {
//	ushort clientID;
//	uint16 size;
//	PACKET_TYPE type;
//};

//struct Packet {
//	PacketHeader header;
//	char data[1024];
//
//	// TEMP
//	Vec3 pos;
//	ushort id;
//};

enum class NETWORK_STATE {
	SINGLE,
	HOST,
	GUEST,
};

struct PacketQueue {
	LockQueue<Packet> toServer;
	LockQueue<Packet> toClient;
};

struct GuestInfo {
	ushort id;
	SOCKET socket;
	shared_ptr<PacketQueue> eventQue = make_shared<PacketQueue>();
};

//struct PlayerData {
//	Vec3 pos;
//	ushort id;
//	// ... Class, Hp, Item, ...
//};
//
//struct GameData {
//	PlayerData playerData[MAX_PLAYER];
//	// ... Enemy, Item, ...
//};

class Network {
public:
	Network();
	~Network() {}

	virtual void Update();

	virtual void Send(Packet packet, int id) {};
	virtual void Send(char* data, int size) {};
	virtual void Send(shared_ptr<char[]> data, int size) {};
	virtual void Send(shared_ptr<char[]> data, int size, int guestId) {};
	//virtual Packet Recv();
	virtual bool Recv(shared_ptr<Packet> packet) { return false; }

	shared_ptr<Packet> PacketProcess(int bufferIdx);

	NETWORK_STATE GetState() { return m_networkState; }
	void SetState(NETWORK_STATE state) { m_networkState = state; }

	int ProcessPlayerID(int id);

	LockQueue<shared_ptr<Packet>> m_receivedPacketQue;

private:
	WSADATA m_wsaData;
	atomic<NETWORK_STATE> m_networkState = NETWORK_STATE::SINGLE;

	atomic<bool> m_isRunning = false;

	Packet m_packetBuffer;

protected:
	Buffer m_buffer[2];

public:
	SOCKET m_socket;
};

class Host : public Network {
public:
	Host();
	~Host() {}

	void Update() override;

	void RunMulti();

	void Stop();

	void MainLoop();
	void WaitLoop();
	void GameLoop();
	void Connection(ushort id);

	void Send(Packet packet, int id) override;
	//void Send(char* data, int size) override;
	void Send(shared_ptr<char[]> data, int size) override;
	void Send(shared_ptr<char[]> data, int size, int guestId) override;
	bool Recv(shared_ptr<Packet> packet) override;

	bool IsThroughPacket(PACKET_TYPE type);

	char* m_myIP;

private:
	vector<GuestInfo> m_guestInfos;
	SOCKET m_listenSocket;

	thread m_mainLoopThread;
	thread m_waitLoopThread;

	float m_timer = 0.f;

	PacketQueue m_eventQue;
};

class Guest : public Network {
public:
	Guest();
	~Guest() {}

	void Update() override;

	void Connect();
	void Sender();
	void Receiver();

	void Send(Packet packet, int id) override;
	void Send(char* data, int size) override;
	void Send(shared_ptr<char[]> data, int size) override;
	void Send(shared_ptr<char[]> data, int size, int guestId) override {};
	bool Recv(shared_ptr<Packet> packet) override;
private:

	// 임시코드
	char* m_serverIP = (char*)"127.0.0.1";

	LockQueue<Packet> m_toServerEventQue;
	LockQueue<pair<shared_ptr<char[]>, ushort>> m_toServerDataQue;
};

class NetworkManager {
	DECLARE_SINGLE(NetworkManager);

public:
	void Initialize();
	void Update();

	void MakeCorrectPacket();

	void RunMulti();

	void ConnectAsGuest();

	NETWORK_STATE GetNetworkState() { return m_network->GetState(); }
	void SetNetworkState(NETWORK_STATE state) { m_network->SetState(state); }

	void Send(Packet packet);
	void Send(char* data, int size);
	void Send(shared_ptr<char[]> data, int size);
	void Send(shared_ptr<char[]> data, int size, int guestId);
	bool Recv(shared_ptr<Packet> packet);

	char* GetMyIP() { return dynamic_cast<Host*>(m_network.get())->m_myIP; }

	ushort m_networkId = 0;

	unique_ptr<Network> m_network;

	atomic<int> m_displayVar = 0;

	bool m_isSend = false;
private:
	chrono::system_clock::time_point m_prevTime;
	double m_remainTime;
};

class NetworkScript : public MonoBehaviour {
public:
	NetworkScript() {}
	virtual ~NetworkScript() {}

	virtual void LateUpdate() override;
};