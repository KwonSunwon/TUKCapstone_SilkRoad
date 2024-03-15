#pragma once

#include "LockQueue.h"
#include "MonoBehaviour.h"

static const int MAX_PLAYER = 2;
static const int SERVER_PORT = 9000;

// global
// 임시로 이렇게 만들긴 했는데...
// 게임에서 필요로 하는 모든 것을
// 저장할 수 있는 구조체에 넣어야 하는거 아닌가 하는...
// 아니면 Packet에 타입을 통해서 루프를 돌면서
// 꺼낸 것을 확인하는 방식으로 해야 하나?
//extern shared_ptr<LockQueue<Packet>> g_packetQueue;

//class Server {
//public:
//	Server() {}
//	~Server() {}
//
//	void Initialize();
//
//	//void Run();
//	void Stop();
//	bool Change(std::string serverCode);
//	void Update();
//
//	// 게스트와 연결 및 쓰레드 생성
//	bool ConnectGuest();
//
//public:
//	friend void Run(Server);
//
//private:
//	bool m_isRunning;
//	std::string m_serverCode;
//
//	// 각각의 게스트와 연결을 유지하기 위한 변수들
//	SOCKET m_guestSocket[MAX_GUEST];
//	SOCKADDR_IN m_guestAddr[MAX_GUEST];
//
//	std::array<std::thread, MAX_GUEST> m_guestThread;
//	size_t m_guestCount = 0;
//
//private:
//	// IP 주소를 코드로 전환
//	char* ServerCodeToIP(std::string serverCode);
//	std::string IPToServerCode(char* ip);
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

/*
* 쓰레드 생성 및 관리
* 쓰레드 동기화에 필요한 Lock 변수 관리
* 게스트와 연결 및 패킷 송수신
* 서버에서 처리해야 하는 게임 루프
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
//	// 데이터 송수신
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

	virtual void Update() {};

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
	//Packet Recv() override;

private:
	vector<GuestInfo> m_guestInfos;
	SOCKET m_listenSocket;

	thread m_mainLoopThread;
	thread m_waitLoopThread;

	// 호스트 클라이언트에서 send(정확히는 push)한 패킷 큐
	PacketQueue m_eventQue;

	// 임시
	array<Packet, 2> m_gameData;
	array<Packet, 2> m_lastGameData;
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

	// 임시코드
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