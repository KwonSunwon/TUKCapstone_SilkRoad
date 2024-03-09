#include "Pch.h"

#include "Network.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"

#pragma region Legacy
/*
void Network::Run()
{
	thread serverThread{ &Network::MainLoop, this };
	m_serverThread = move(serverThread);
	m_isRunning = true;
}

void Network::Stop()
{
	// Host에서 Guest 플레이로 전환할 때 호출
	if (m_isRunning) {
		m_isRunning = false;
		m_serverThread.join();

		SetState(NETWORK_STATE::GUEST_DISCONNECTED);
	}
}

void Network::Initialize()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
		throw runtime_error("Fail initialize WSADATA");
	}
	Run();
}

void Network::Host_RunMulti()
{
	SetState(NETWORK_STATE::HOST_MULTI);

	//if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
	//	throw runtime_error("Fail initialize WSADATA");
	//}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize listen socket");
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(9000);
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		throw runtime_error("Fail bind listen socket");
	};

	if (listen(listenSocket, MAX_GUEST) == SOCKET_ERROR) {
		throw runtime_error("Fail listen listen socket");
	}

	m_listenSocket = move(listenSocket);

	thread waitThread{ &Network::Host_WaitLoop, this };
	m_waitThread = move(waitThread);
}

void Network::MainLoop()
{
	Packet inPacket, outPacket;
	while (m_isRunning) {
		// 클라 쓰레드에서 패킷을 받아 갱신
		while (m_packetQueue.in.TryPop(inPacket)) {
			// 패킷 처리
		}

		// 게스트가 보낸 패킷을 받아 갱신
		for (auto& guest : m_guestInfo) {
			while (guest.packetQueue.in.TryPop(inPacket)) {
				// 패킷 처리
			}
		}

		// outPacket = GameLoop();

		// 클라 쓰레드로 패킷을 보냄
		m_packetQueue.out.Push(outPacket);

		// 게스트에게 패킷을 보냄
		for (auto& guest : m_guestInfo) {
			guest.packetQueue.out.Push(outPacket);
		}

		// Test
		// 게임 내용 완전 작성 전 임시로 사용
		// 모든 플레이어의 위치를 1/60초마다 전송
		// 모든 플레이어가 각각 플레이어의 위치를 받아서 그대로 그려줌
	}
}

void Network::Host_WaitLoop()
{
	struct sockaddr_in clientAddr;
	int addrLen;
	m_guestInfo.reserve(MAX_GUEST);
	int playerCount = 0;
	while (m_isRunning && m_networkState == NETWORK_STATE::HOST_MULTI) {
		// WaitForClients
		SOCKET tempSocket = ::accept(m_listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
		if (tempSocket == INVALID_SOCKET) {
			throw runtime_error("Fail accept client socket");
		}
		ClientInfo clientInfo{ playerCount++, tempSocket };
		m_guestInfo.emplace_back(clientInfo);

		// 호스트서버-게스트 통신 유지
		thread guestConnectionThread;
	}
}

void Network::Guest_Connect()
{
	// 기존 서버 쓰레드 종료
	Stop();

	Guest tempGuest;

	// 관련된 소켓 초기화
	tempGuest.socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tempGuest.socket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize socket");
	}

	// 호스트의 IP주소를 통해 호스트 서버에 접속
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(9000);
	if (connect(tempGuest.socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		throw runtime_error("Fail connect server socket");
	}

	// 호스트와 데이터를 송수신할 수 있는 쓰레드 생성
	thread guestConnectionThread{ &Network::Guest_Connection, this };
	tempGuest.connectionThread = move(guestConnectionThread);

	m_guestInfo.emplace_back(tempGuest);
}

// 각각의 게스트마다 쓰레드를 가짐
// 그러면 쓰레드 속에 정보를 저장해 줘야 하나?...
// Network 클래스 내부에 Guest, Host(Server) 클래스를 만들어서
// 사용/관리 해야 할 것 같음...
void Network::Guest_Connection()
{
	// ushort id = id; ???

	// 서버와 게스트 패킷 송수신
	// 호스트와 연결이 지속되는 동안
	while (true) {

	}
}

void Network::Send(Packet packet)
{
	if (m_networkState == NETWORK_STATE::HOST_MULTI) {
		// 호스트 서버에서 게스트로 패킷 전송
		PushPacket(packet);
	}
	else if (m_networkState == NETWORK_STATE::GUEST_CONNECTED) {
		// 게스트에서 호스트 서버로 패킷 전송
		SendPacket(packet);
	}
}

Packet Network::Recv()
{
	if (m_networkState == NETWORK_STATE::HOST_MULTI) {
		// 호스트 서버에서 게스트로부터 패킷 수신
		return PopPacket();
	}
	else if (m_networkState == NETWORK_STATE::GUEST_CONNECTED) {
		// 게스트에서 호스트 서버로부터 패킷 수신
		return RecvPacket();
	}

	return Packet();
}

void Network::SendPacket(Packet packet)
{
	// 게스트에서 호스트 서버로 패킷 전송
	Packet tempPacket;
	for (auto& guest : m_guestInfo) {
		while (guest.toHostEvent.TryPop()) {
			if (send(guest.socket, reinterpret_cast<char*>(&tempPacket), sizeof(Packet), 0) == SOCKET_ERROR) {
				throw runtime_error("Fail send packet");
			}
		}
	}
}

Packet Network::RecvPacket()
{
	// 게스트에서 호스트 서버로부터 패킷 수신
	Packet tempPacket;
	for (auto& guest : m_guestInfo) {
		while (true) {
			int returnValue = recv(guest.socket, reinterpret_cast<char*>(&tempPacket), sizeof(Packet), 0);
			if (returnValue == SOCKET_ERROR) {
				throw runtime_error("Fail recv packet");
			}
			else {
				return tempPacket;
			}
		}
	}
	return Packet();
}

void Network::PushPacket(Packet packet)
{
	m_packetQueue.Push(packet);
}

Packet Network::PopPacket()
{
	Packet packet;
	m_packetQueue.WaitPop(packet);
	return packet;
}

*/
#pragma endregion

Network::Network()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
		throw runtime_error("Fail initialize WSADATA");
	}
}

#pragma region Host
Host::Host() : Network()
{
	m_mainLoopThread = thread{ &Host::MainLoop, this };

	m_gameData[0].pos = Vec3{ 0, 0, 0 };
	m_gameData[0].id = 0;
	m_gameData[1].pos = Vec3{ 0, 0, 0 };
	m_gameData[1].id = 1;
	m_lastGameData[0].pos = Vec3{ 0, 0, 0 };
	m_lastGameData[0].id = 0;
	m_lastGameData[1].pos = Vec3{ 0, 0, 0 };
	m_lastGameData[1].id = 1;
}

void Host::MainLoop()
{
	while (GetState() != NETWORK_STATE::GUEST) {
		{
			Packet packet;
			if (GetState() == NETWORK_STATE::HOST) {
				// 게스트가 보낸 패킷을 받아 갱신
				for (auto& guest : m_guestInfos) {
					while (guest.eventQue->toServer.TryPop(packet))
						m_gameData[guest.id] = packet;
				}
			}
			// 호스트 클라이언트가 푸시한 패킷을 받아 갱신
			string str = "MainLoop: Queue Size - " + to_string(m_eventQue.toServer.Size()) + "\n";
			while (m_eventQue.toServer.TryPop(packet)) {
				m_gameData[0] = packet;
			}
		}
		GameLoop();
		{
			//Packet packet;
			//if (GetState() == NETWORK_STATE::HOST) {
			//	// 게스트에게 보낼 패킷을 큐에 푸시
			//	for (auto& guest : m_guestInfos)
			//		guest.eventQue.get()->toClient.Push(packet);
			//}
			//// 호스트 클라이언트에게 보낼 패킷을 큐에 푸시	
			//m_eventQue.toClient.Push(packet);
		}
	}
	OutputDebugString(L"Host MainLoop End\n");
}

void Host::GameLoop()
{
	for (int i = 0; i < 2; i++) {
		if (m_gameData[i].pos != m_lastGameData[i].pos) {
			m_lastGameData[i] = m_gameData[i];

			for (auto& guest : m_guestInfos) {
				guest.eventQue->toClient.Push(m_gameData[i]);
			}
			m_eventQue.toClient.Push(m_gameData[i]);
		}
	}
}

void Host::Update()
{
	int count = 0;
	Packet packet;
	while (m_eventQue.toClient.TryPop(packet)) {
		// 게임에 적용
		if (packet.id == 0) {
			string str = "Update: Packet " + to_string(count++) + " - " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
			OutputDebugStringA(str.c_str());
		}
		if (packet.id == 1) {
			string str2 = "Update: Packet2 " + to_string(count++) + " - " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
			OutputDebugStringA(str2.c_str());
			break;
		}

		shared_ptr<GameObject> player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer();
		player->GetTransform()->SetLocalPosition(packet.pos);
	}
}

void Host::RunMulti()
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize listen socket");
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(9000);
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		throw runtime_error("Fail bind listen socket");
	};

	if (listen(listenSocket, MAX_GUEST) == SOCKET_ERROR) {
		throw runtime_error("Fail listen listen socket");
	}

	m_listenSocket = move(listenSocket);

	m_waitLoopThread = thread{ &Host::WaitLoop, this };
}

void Host::Stop()
{
	//m_waitLoopThread.join();
	//m_mainLoopThread.join();
}

void Host::WaitLoop()
{
	struct sockaddr_in clientAddr;
	int addrLen;
	m_guestInfos.reserve(MAX_GUEST);
	int playerCount = 0;
	while (GetState() == NETWORK_STATE::HOST) {
		// WaitForClients
		addrLen = sizeof(clientAddr);
		SOCKET tempSocket = ::accept(m_listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (tempSocket == INVALID_SOCKET) {
			throw runtime_error("Fail accept client socket");
		}

		DWORD optval = 10;
		setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));

		GuestInfo guest;
		guest.id = ++playerCount;
		guest.socket = move(tempSocket);
		m_guestInfos.emplace_back(guest);

		thread connectionThread = thread{ &Host::Connection, this, guest.id };
		connectionThread.detach();
	}
	closesocket(m_listenSocket);
	OutputDebugString(L"Host WaitLoop End\n");
}

void Host::Connection(ushort id)
{
	SOCKET socket;
	shared_ptr<PacketQueue> eventQue;
	for (auto& guest : m_guestInfos) {
		if (guest.id == id) {
			socket = guest.socket;
			eventQue = guest.eventQue;
			break;
		}
	}

	// 게스트와 패킷 송수신
	int retval;
	while (GetState() == NETWORK_STATE::HOST) {
		Packet packet;
		// 서버에서 게스트로 보내는 패킷
		while (eventQue->toClient.TryPop(packet)) {
			retval = send(socket, (char*)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR) {

			}
		}
		// 게스트에서 서버로 보내는 패킷
		while (true) {
			retval = recv(socket, (char*)&packet, sizeof(packet), 0);
			if (retval > 0) {
				eventQue->toServer.Push(packet);
			}
			if (retval < 0) {
				break;
			}
		}
	}
}
void Host::Send(Packet packet)
{
	m_eventQue.toServer.Push(packet);
}
#pragma endregion

#pragma region Guest
Guest::Guest() : Network()
{

}

void Guest::Connect()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize socket");
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, m_serverIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVER_PORT);
	int retval = connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		//err_display(retval);
		throw runtime_error("Fail connect server");
	}

	DWORD optval = 5;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
}

void Guest::Update()
{
	// 패킷을 받고 게임에 적용하는 부분
	int count = 0;
	int retval;
	shared_ptr<Packet> packet = make_shared<Packet>();
	while (Recv(packet)) {
		string str = "Update: Packet " + to_string(count++) + " - " + to_string(packet->pos.x) + ", " + to_string(packet->pos.y) + ", " + to_string(packet->pos.z) + "\n";
		OutputDebugStringA(str.c_str());

		if (packet->id == 0)
			break;

		shared_ptr<GameObject> player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer();
		player->GetTransform()->SetLocalPosition(packet->pos);
	}
}

void Guest::Send(Packet packet)
{
	int retval = send(m_socket, (char*)&packet, sizeof(Packet), 0);
	if (retval < 0) {
		//err_display(retval);
	}
}

bool Guest::Recv(shared_ptr<Packet> packet)
{
	int retval = recv(m_socket, (char*)packet.get(), sizeof(Packet), 0);
	if (retval < 0) {
		return false;
	}
	return true;
}
#pragma endregion

#pragma region Network
void NetworkManager::Initialize()
{
	m_network = make_unique<Host>();
}

void NetworkManager::Update()
{
	m_network.get()->Update();
}

void NetworkManager::RunMulti()
{
	if (GetNetworkState() == NETWORK_STATE::SINGLE) {
		SetNetworkState(NETWORK_STATE::HOST);

		dynamic_cast<Host*>(m_network.get())->RunMulti();
	}
}

void NetworkManager::ConnectAsGuest()
{
	if (GetNetworkState() != NETWORK_STATE::GUEST) {
		SetNetworkState(NETWORK_STATE::GUEST);

		dynamic_cast<Host*>(m_network.get())->Stop();

		m_network.release();
		m_network = make_unique<Guest>();

		dynamic_cast<Guest*>(m_network.get())->Connect();
	}
}

void NetworkScript::LateUpdate()
{
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		GET_SINGLE(NetworkManager)->RunMulti();
		m_id = 0;
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
		// 호스트에서 게스트로 전환
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
		m_id = 1;
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		// 게스트에서 호스트로 전환
	}

	Packet packet;
	shared_ptr<GameObject> player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer();
	packet.pos = player->GetTransform()->GetLocalPosition();
	packet.id = m_id;
	//string str = "Send: " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
	//OutputDebugStringA(str.c_str());
	GET_SINGLE(NetworkManager)->Send(packet);
}
#pragma endregion