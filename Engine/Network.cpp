#include "Pch.h"

#include "Network.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "NetworkPlayer.h"
#include "Enemy.h"

Network::Network()
{
	if(WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
		throw runtime_error("Fail initialize WSADATA");
	}
}

/*
* Update
*  패킷을 받아서 게임에 적용
*/
void Network::Update()
{
	auto objects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();
	auto players = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers();

	shared_ptr<Packet> packet;
	//while(Recv(m_packetBuffer)) {
	while(m_receivedPacketQue.TryPop(packet)) {
		switch(packet->m_type) {
		case PACKET_TYPE::PT_MOVE:
			/*for(auto& player : players) {
				if(!player)
					continue;
				if(player->GetID() == packet->m_targetId) {
					player->GetTransform()->SetLocalPosition(reinterpret_pointer_cast<MovePacket>(packet)->m_position);
				}
			}*/
			players[packet->m_targetId]->GetTransform()->SetLocalPosition(reinterpret_pointer_cast<MovePacket>(packet)->m_position);
			break;
		case PACKET_TYPE::PT_PLAYER:
			GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->ProcessPacket(reinterpret_pointer_cast<PlayerPacket>(packet));
			break;
		case PACKET_TYPE::PT_ENEMY:
			GET_SINGLE(SceneManager)->GetActiveScene()->m_enemies[packet->m_targetId]->ProcessPacket(reinterpret_pointer_cast<EnemyPacket>(packet));
			break;
		default:
			break;
		}
	}
}

#pragma region Host
Host::Host() : Network()
{
	m_mainLoopThread = thread{ &Host::MainLoop, this };
}

void Host::MainLoop()
{
	/*Packet packet;
	while(GetState() == NETWORK_STATE::HOST) {
		if(m_guestInfos.empty()) {
			continue;
		}
		for(auto& guest : m_guestInfos) {
			while(guest.eventQue->toServer.TryPop(packet)) {
				m_receivedPacketQue.Push(make_shared<Packet>(packet));
			}
		}
	}*/
}

void Host::GameLoop()
{

}

void Host::Update()
{
	Network::Update();
}

void Host::RunMulti()
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize listen socket");
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(9000);
	if(::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		throw runtime_error("Fail bind listen socket");
	};

	if(listen(listenSocket, MAX_PLAYER) == SOCKET_ERROR) {
		throw runtime_error("Fail listen listen socket");
	}

	// Get my ip address
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	struct hostent* host = gethostbyname(hostname);
	m_myIP = inet_ntoa(*(struct in_addr*)*host->h_addr_list);

	GET_SINGLE(NetworkManager)->m_displayVar = 1;

	m_listenSocket = move(listenSocket);

	GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->m_myNetworkId = 1;
	GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[1]->m_myNetworkId = 2;

	m_waitLoopThread = thread{ &Host::WaitLoop, this };
}

void Host::WaitLoop()
{
	struct sockaddr_in clientAddr;
	int addrLen;
	m_guestInfos.reserve(MAX_PLAYER);
	int playerCount = 0;
	while(GetState() == NETWORK_STATE::HOST) {
		// WaitForClients
		addrLen = sizeof(clientAddr);
		SOCKET tempSocket = ::accept(m_listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if(tempSocket == INVALID_SOCKET) {
			throw runtime_error("Fail accept client socket");
		}

		GuestInfo guest;
		guest.id = ++playerCount;
		guest.socket = move(tempSocket);
		m_guestInfos.emplace_back(guest);

		InitPacket initPacket;
		initPacket.m_networkId = guest.id;
		send(guest.socket, (char*)&initPacket, sizeof(InitPacket), 0);

		DWORD optval = TIMEOUT;
		setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
		optval = TRUE;
		setsockopt(tempSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

		thread connectionThread = thread{ &Host::Connection, this, guest.id };
		connectionThread.detach();

		OutputDebugString(L"Connect New Guest\n");

		GET_SINGLE(NetworkManager)->m_displayVar = 2;
	}
	closesocket(m_listenSocket);
	OutputDebugString(L"Host WaitLoop End\n");
}

// session
void Host::Connection(ushort id)
{
	SOCKET socket;
	shared_ptr<PacketQueue> eventQue;
	for(auto& guest : m_guestInfos) {
		if(guest.id == id) {
			socket = guest.socket;
			eventQue = guest.eventQue;
			break;
		}
	}

	chrono::steady_clock::time_point startTime;
	chrono::steady_clock::time_point endTime;
	float elapsedTime;
	float remainTime;

	// 게스트와 패킷 송수신
	int retval;
	while(GetState() == NETWORK_STATE::HOST) {
		startTime = chrono::steady_clock::now();

		{
			Packet packet;
			// 서버에서 게스트로 보내는 패킷
			while(eventQue->toClient.TryPop(packet)) {
				retval = send(socket, (char*)&packet, packet.m_size, 0);
				if(retval == SOCKET_ERROR) {
					// disconnect
					OutputDebugString(L"Disconnect in Connection send\n");
					closesocket(socket);
					break;
				}
			}
		}

		// 게스트에서 서버로 보내는 패킷
		shared_ptr<Packet> packet = nullptr;
		shared_ptr<char[]> buffer = make_shared<char[]>(BUFFER_SIZE);

		// Write received data to buffer
		retval = recv(socket, buffer.get(), BUFFER_SIZE - m_buffer.Size(), 0);
		if(retval > 0) {
			m_buffer.Write(buffer.get(), retval);

			ushort packetSize = m_buffer.Peek();
			while(packetSize <= m_buffer.Size()) {
				// Save packet to m_receivedPacketQue for apply to game
				PACKET_TYPE packetType = static_cast<PACKET_TYPE>(m_buffer.Peek(2));
				switch(packetType) {
				case PACKET_TYPE::PT_NONE:
					break;
				case PACKET_TYPE::PT_INIT:
					packet = make_shared<InitPacket>();
					break;
				case PACKET_TYPE::PT_MOVE:
					packet = make_shared<MovePacket>();
					break;
				case PACKET_TYPE::PT_PLAYER:
					packet = make_shared<PlayerPacket>();
					break;
				case PACKET_TYPE::PT_ENEMY:
					packet = make_shared<EnemyPacket>();
					break;
				}
				m_buffer.Read(reinterpret_cast<char*>(packet.get()), packet->m_size);

				m_receivedPacketQue.Push(packet);

				packetSize = 0;
				packet.reset();

				if(m_buffer.Empty())
					break;

				packetSize = m_buffer.Peek();
			}
		}
		endTime = chrono::steady_clock::now();
		elapsedTime = chrono::duration<float>(endTime - startTime).count();
		remainTime = SEND_PACKET_PER_SEC - elapsedTime;
		if(remainTime > 0.f) {
			this_thread::sleep_for(chrono::duration<float>(remainTime));
		}
	}
}
void Host::Send(Packet packet, int id)
{
	//m_eventQue.toServer.Push(packet);
	//packet.header.clientID = id;
	for(auto& guest : m_guestInfos) {
		guest.eventQue->toClient.Push(packet);
	}

}

void Host::Send(shared_ptr<char[]> data, int size)
{
	if(m_guestInfos.empty())
		return;
	for(auto& guest : m_guestInfos) {
		send(guest.socket, data.get(), size, 0);
	}
}

bool Host::Recv(shared_ptr<Packet> packet)
{
	/*if(m_eventQue.toClient.TryPop(*packet.get()))
		return true;
	return false;*/

	if(m_eventQue.toServer.TryPop(*packet))
		return true;
	return false;
}
#pragma endregion

#pragma region Guest
Guest::Guest() : Network()
{

}

void Guest::Connect()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_socket == INVALID_SOCKET) {
		throw runtime_error("Fail initialize socket");
	}

	ifstream file("HostIp.txt");
	if(!file.is_open()) {
		OutputDebugString(L"Fail open HostIp.txt\n");
	}
	else {
		char buffer[16];
		file.getline(buffer, 16);
		m_serverIP = buffer;
		file.close();
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, m_serverIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVER_PORT);
	int retval = connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(retval == SOCKET_ERROR) {
		//err_display(retval);
		throw runtime_error("Fail connect server");
	}

	InitPacket initPacket;
	recv(m_socket, (char*)&initPacket, sizeof(InitPacket), 0);
	GET_SINGLE(NetworkManager)->m_networkId = initPacket.m_networkId;

	DWORD optval = TIMEOUT;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
	optval = TRUE;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));
	GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->m_myNetworkId = 0;
	if(initPacket.m_networkId == 1) {
		GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[1]->m_myNetworkId = 2;
	}
	else {
		GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[1]->m_myNetworkId = 1;
	}

	thread senderThread = thread{ &Guest::Sender, this };
	thread receiverThread = thread{ &Guest::Receiver, this };
	senderThread.detach();
	receiverThread.detach();

	OutputDebugString(L"Connect Host\n");
}

void Guest::Sender()
{
	int retval;
	Packet packet;

	while(GetState() == NETWORK_STATE::GUEST) {
		//while(m_toServerEventQue.TryPop(packet)) {
		//	retval = send(m_socket, (char*)&packet, packet.m_size, 0);
		//	if(retval == SOCKET_ERROR) {
		//		// disconnect
		//		OutputDebugString(L"Disconnect in Sender()\n");
		//		closesocket(m_socket);
		//	}
		//}
//#define DEBUG
//#ifdef DEBUG
//		shared_ptr<PlayerPacket> testPlayerPacket = make_shared<PlayerPacket>();
//		testPlayerPacket->m_position = { 3500.f, 1500.f, 2500.f };
//		testPlayerPacket->m_rotation = { 0.f, 0.f, 0.f };
//		testPlayerPacket->m_velocity = { 100.f, 0.f, 0.f };
//		testPlayerPacket->m_animationIndex = 2;
//
//		//m_receivedPacketQue.Push(testPlayerPacket);
//		send(m_socket, (char*)testPlayerPacket.get(), testPlayerPacket->m_size, 0);
//#endif // DEBUG
		pair<shared_ptr<char[]>, ushort> data;
		while(m_toServerDataQue.TryPop(data)) {
			retval = send(m_socket, data.first.get(), data.second, 0);
			if(retval == SOCKET_ERROR) {
				// disconnect
				OutputDebugString(L"Disconnect in Sender()\n");
				closesocket(m_socket);
			}
		}
		this_thread::yield();
	}
}

void Guest::Receiver()
{
	// 1/60초마다 호스트에게서 패킷을 받아와 m_receivedPacketQue에 저장
	int retval;
	Packet packet;

	chrono::steady_clock::time_point startTime;
	chrono::steady_clock::time_point endTime;
	float elapsedTime;
	float remainTime;

	while(GetState() == NETWORK_STATE::GUEST) {
		startTime = chrono::steady_clock::now();

		shared_ptr<Packet> packet = nullptr;
		shared_ptr<char[]> buffer = make_shared<char[]>(BUFFER_SIZE);

		// Write received data to buffer
		retval = recv(m_socket, buffer.get(), BUFFER_SIZE - m_buffer.Size(), 0);
		if(retval > 0) {
			m_buffer.Write(buffer.get(), retval);

			ushort packetSize = m_buffer.Peek();
			while(packetSize <= m_buffer.Size()) {
				// Save packet to m_receivedPacketQue for apply to game
				PACKET_TYPE packetType = static_cast<PACKET_TYPE>(m_buffer.Peek(2));
				switch(packetType) {
				case PACKET_TYPE::PT_NONE:
					break;
				case PACKET_TYPE::PT_INIT:
					packet = make_shared<InitPacket>();
					break;
				case PACKET_TYPE::PT_MOVE:
					packet = make_shared<MovePacket>();
					break;
				case PACKET_TYPE::PT_PLAYER:
					packet = make_shared<PlayerPacket>();
					break;
				case PACKET_TYPE::PT_ENEMY:
					packet = make_shared<EnemyPacket>();
					break;
				}
				m_buffer.Read(reinterpret_cast<char*>(packet.get()), packet->m_size);

				m_receivedPacketQue.Push(packet);

				packetSize = 0;
				packet.reset();

				if(m_buffer.Empty())
					break;

				packetSize = m_buffer.Peek();
			}
		}
		endTime = chrono::steady_clock::now();
		elapsedTime = chrono::duration<float>(endTime - startTime).count();
		remainTime = SEND_PACKET_PER_SEC - elapsedTime;
		if(remainTime > 0.f) {
			this_thread::sleep_for(chrono::duration<float>(remainTime));
		}
	}
}

void Guest::Update()
{
	Network::Update();
}

void Guest::Send(Packet packet, int id)
{
	m_toServerEventQue.Push(packet);
}

void Guest::Send(char* data, int size)
{
	int retval = send(m_socket, data, size, 0);
	if(retval == SOCKET_ERROR) {
		// disconnect
		OutputDebugString(L"Disconnect in Send()\n");
		closesocket(m_socket);
	}
}

void Guest::Send(shared_ptr<char[]> data, int size)
{
	m_toServerDataQue.Push(make_pair(data, size));
}

bool Guest::Recv(shared_ptr<Packet> packet)
{
	int retval = recv(m_socket, (char*)packet.get(), sizeof(Packet), 0);
	if(retval < 0) {
		return false;
	}
	return true;
}
#pragma endregion

#pragma region Network
void NetworkManager::Initialize()
{
	m_network = make_unique<Network>();
}

void NetworkManager::Update()
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Update();
}

void NetworkManager::MakeCorrectPacket()
{
	if(GetNetworkState() == NETWORK_STATE::SINGLE)
		return;

	// Make correct position, health, etc packet of all objects
	// It called by 1sec timer
	// Send packet to all clients
	auto objects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	for(auto& object : objects) {

	}
}

void NetworkManager::RunMulti()
{
	if(GetNetworkState() == NETWORK_STATE::SINGLE) {
		m_network.release();
		m_network = make_unique<Host>();

		SetNetworkState(NETWORK_STATE::HOST);
		dynamic_cast<Host*>(m_network.get())->RunMulti();
	}
}

void NetworkManager::ConnectAsGuest()
{
	if(GetNetworkState() != NETWORK_STATE::GUEST) {
		m_network.release();
		m_network = make_unique<Guest>();

		SetNetworkState(NETWORK_STATE::GUEST);
		dynamic_cast<Guest*>(m_network.get())->Connect();
	}
}

void NetworkManager::Send(Packet packet)
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Send(packet, m_networkId);
}

void NetworkManager::Send(char* data, int size)
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Send(data, size);
}

void NetworkManager::Send(shared_ptr<char[]> data, int size)
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Send(data, size);
}

bool NetworkManager::Recv(shared_ptr<Packet> packet)
{
	if(GetNetworkState() == NETWORK_STATE::SINGLE)
		return false;
	return m_network->Recv(packet);
}

void NetworkScript::LateUpdate()
{
	if(INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		GET_SINGLE(NetworkManager)->m_networkId = 0;
		GET_SINGLE(NetworkManager)->RunMulti();
	}

	if(INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
	}

	if(INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		/*Packet packet;
		packet.header.type = PACKET_TYPE::NET;
		packet.id = -1;
		GET_SINGLE(NetworkManager)->Send(packet);*/
	}
}
#pragma endregion