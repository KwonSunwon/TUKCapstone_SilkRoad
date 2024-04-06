#pragma once

#include "LockQueue.h"
#include "MonoBehaviour.h"

static const int MAX_PLAYER = 2;
static const int SERVER_PORT = 9000;
static const int TIMEOUT = 5;

#define SEND_PACKET_PER_SEC 1.f / 60.f

// Packet Type
enum class PACKET_TYPE {
	PLAYER,
	ENEMY,
	ITEM,
	// ...
	NET,
	END
};

// Packet Struct
struct PacketHeader {
	uint16 size;
	PACKET_TYPE type;
};

struct Packet {
	PacketHeader header;
	char data[1024];

	// TEMP
	Vec3 pos;
	ushort id;
};

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

struct PlayerData {
	Vec3 pos;
	ushort id;
	// ... Class, Hp, Item, ...
};

struct GameData {
	PlayerData playerData[MAX_PLAYER];
	// ... Enemy, Item, ...
};

/*
* ������ ���� �� ����
* ������ ����ȭ�� �ʿ��� Lock ���� ����
* �Խ�Ʈ�� ���� �� ��Ŷ �ۼ���
* �������� ó���ؾ� �ϴ� ���� ����
*/
//class Network {
//	DECLARE_SINGLE(Network);
//
//public:
//	void Initialize();
//
//	// thread loop functions
//	void MainLoop();
//	void GameLoop();
//
//	void Host_WaitLoop();
//
//	// interface
//	void Run();
//	void Stop();
//
//	void Host_RunMulti();
//
//	void Guest_Connection();
//	void Guest_Connect();
//
//	// ������ �ۼ���
//	void Send(Packet packet);
//	Packet Recv();
//
//	void SendPacket(Packet packet);
//	Packet RecvPacket();
//
//	void PushPacket(Packet packet);
//	Packet PopPacket();
//
//	void SendPacketToServer(Packet packet);
//	Packet RecvPacketFromServer();
//
//	void SendPacketToClient(Packet packet);
//	Packet RecvPacketFromClient();
//
//	NETWORK_STATE GetState() { return m_networkState; }
//	void SetState(NETWORK_STATE state) { m_networkState = state; }
//
//private:
//	WSADATA m_wsaData;
//	SOCKET m_listenSocket;
//
//	vector<Guest> m_guestInfo;
//
//	atomic<bool> m_isRunning{ false };
//
//	thread m_serverThread;
//	thread m_waitThread;
//
//	NETWORK_STATE m_networkState = NETWORK_STATE::HOST_SINGLE;
//
//	PacketQueue m_packetQueue;
//};

class Network {
public:
	Network();
	~Network() {}

	virtual void Update();

	virtual void Send(Packet packet) {};
	//virtual Packet Recv();
	virtual bool Recv(shared_ptr<Packet> packet) { return false; }

	NETWORK_STATE GetState() { return m_networkState; }
	void SetState(NETWORK_STATE state) { m_networkState = state; }

private:
	WSADATA m_wsaData;
	atomic<NETWORK_STATE> m_networkState = NETWORK_STATE::SINGLE;

	atomic<bool> m_isRunning = false;
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

	void Send(Packet packet) override;
	bool Recv(shared_ptr<Packet> packet) override;

private:
	vector<GuestInfo> m_guestInfos;
	SOCKET m_listenSocket;

	thread m_mainLoopThread;
	thread m_waitLoopThread;

	float m_timer = 0.f;

	// ȣ��Ʈ Ŭ���̾�Ʈ���� send(��Ȯ���� push)�� ��Ŷ ť
	PacketQueue m_eventQue;

	// �ӽ�
	//array<Packet, 2> m_gameData;
	array<Packet, 2> m_lastGameData;

	GameData m_gameData;

	queue<Packet> m_gameLoopEventQue;
	queue<Packet> m_outGameLoopEventQue;
};

class Guest : public Network {
public:
	Guest();
	~Guest() {}

	void Update() override;

	void Connect();

	void Send(Packet packet) override;
	bool Recv(shared_ptr<Packet> packet) override;

private:
	SOCKET m_socket;

	// �ӽ��ڵ�
	char* m_serverIP = (char*)"127.0.0.1";

	shared_ptr<queue<Packet>> m_toClientEventQue;
};

class NetworkManager {
	DECLARE_SINGLE(NetworkManager);

public:
	void Initialize();
	void Update();

	void RunMulti();

	void ConnectAsGuest();

	NETWORK_STATE GetNetworkState() { return m_network.get()->GetState(); }
	void SetNetworkState(NETWORK_STATE state) { m_network.get()->SetState(state); }

	void Send(Packet packet) { m_network->Send(packet); }
	bool Recv(shared_ptr<Packet> packet) { return m_network.get()->Recv(packet); }

	int m_id = 0;

private:
	unique_ptr<Network> m_network;
};

class NetworkScript : public MonoBehaviour {
public:
	NetworkScript() {}
	virtual ~NetworkScript() {}

	virtual void LateUpdate() override;

private:
	ushort m_id = 0;
};