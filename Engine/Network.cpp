#include "Pch.h"

#include "Network.h"
#include "Input.h"

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
}

void Host::MainLoop()
{
	// 여기서 게임 루프 처리
	//Packet packet;
	//while (m_packetQueue.TryPop(packet)) {
	//	// 게임에 적용
	//}
	//for (auto& guest : m_guestInfos) {
	//	while (guest.packetQueue.in.TryPop(packet)) {
	//		// 게임에 적용
	//	}
	//}

	//GameLoop();

	//for (auto& guest : m_guestInfos) {
	//	guest.packetQueue.out.Push(packet);
	//}
	//m_packetQueue.Push(packet);
}

void Host::GameLoop()
{

}

void Host::Update()
{
	Packet packet;
	while (m_packetQueue.TryPop(packet)) {
		// 게임에 적용
	}
}

void Host::RunMulti()
{
	m_isMultiRunning = true;

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

void Host::WaitLoop()
{
	struct sockaddr_in clientAddr;
	int addrLen;
	m_guestInfos.reserve(MAX_GUEST);
	int playerCount = 0;
	while (m_isMultiRunning) {
		// WaitForClients
		addrLen = sizeof(clientAddr);
		SOCKET tempSocket = ::accept(m_listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (tempSocket == INVALID_SOCKET) {
			throw runtime_error("Fail accept client socket");
		}

		DWORD optval = 10;
		setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));

		GuestInfo guest;
		guest.id = playerCount++;
		guest.socket = move(tempSocket);
		m_guestInfos.emplace_back(guest);

		thread connectionThread = thread{ &Host::Connection, this, guest.id };
		connectionThread.detach();
	}
}

//thread_local shared_ptr<PacketQueue> tl_packetQueue = make_shared<PacketQueue>();

void Host::Connection(ushort id)
{
	// 게스트와 패킷 송수신
	int retval;
	for (auto& guest : m_guestInfos) {
		if (guest.id == id) {
			//while (true) {
			//	Packet packet;
			//	// 서버에서 게스트로 보내는 패킷
			//	while (tl_packetQueue.get()->out.TryPop(packet)) {
			//		retval = send(guest.socket, (char*)&packet, sizeof(packet), 0);
			//		if (retval == SOCKET_ERROR) {

			//		}
			//	}
			//	// 게스트에서 서버로 보내는 패킷
			//	while (true) {
			//		retval = recv(guest.socket, (char*)&packet, sizeof(packet), 0);
			//		if (retval > 0) {
			//			tl_packetQueue.get()->in.Push(packet);
			//		}
			//		if (retval < 0) {
			//			break;
			//		}
			//	}
			//}
		}
	}
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

	DWORD optval = 10;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
}

void Guest::Update()
{
	// 패킷을 받고 게임에 적용하는 부분
	int retval;
	shared_ptr<Packet> packet = make_shared<Packet>();
	while (Recv(packet)) {
		// 게임에 적용
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
		dynamic_cast<Host*>(m_network.get())->RunMulti();

		SetNetworkState(NETWORK_STATE::HOST);
	}
}

void NetworkManager::ConnectAsGuest()
{
	if (GetNetworkState() == NETWORK_STATE::SINGLE) {
		m_network.reset();
		m_network = make_unique<Guest>();

		dynamic_cast<Guest*>(m_network.get())->Connect();

		SetNetworkState(NETWORK_STATE::GUEST);
	}
}

void NetworkScript::LateUpdate()
{
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		GET_SINGLE(NetworkManager)->RunMulti();
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
		// 호스트에서 게스트로 전환
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		// 게스트에서 호스트로 전환
	}
}
#pragma endregion