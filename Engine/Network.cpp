#include "Pch.h"

#include "Network.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"

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

	//while(Recv(m_packetBuffer)) {
	while(m_receivedPacketQue.TryPop(m_packetBuffer)) {
		switch(m_packetBuffer.header.type) {
		case PACKET_TYPE::PLAYER:
			players[m_packetBuffer.id]->GetTransform()->SetLocalPosition(m_packetBuffer.pos);
			break;
		case PACKET_TYPE::ENEMY:
			// find target object by id
			for(auto& object : objects) {
				if(object->GetID() == m_packetBuffer.id) {
					// apply packet data to object
					break;
				}
			}
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
	Packet packet;
	while(true) {
		for(auto& guest : m_guestInfos) {
			while(guest.eventQue->toServer.TryPop(packet)) {
				m_receivedPacketQue.Push(packet);
			}
		}
	}
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

	m_listenSocket = move(listenSocket);

	m_waitLoopThread = thread{ &Host::WaitLoop, this };
}

void Host::Stop()
{
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

		DWORD optval = TIMEOUT;
		setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
		optval = TRUE;
		setsockopt(tempSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

		GuestInfo guest;
		guest.id = ++playerCount;
		guest.socket = move(tempSocket);
		m_guestInfos.emplace_back(guest);

		thread connectionThread = thread{ &Host::Connection, this, guest.id };
		connectionThread.detach();

		//GET_SINGLE(SceneManager)->AddPlayer(playerCount);
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

		Packet packet;
		// 서버에서 게스트로 보내는 패킷
		while(eventQue->toClient.TryPop(packet)) {
			retval = send(socket, (char*)&packet, sizeof(packet), 0);
			if(retval == SOCKET_ERROR) {
				// disconnect
				OutputDebugString(L"Disconnect\n");
				closesocket(socket);
				break;
			}
		}
		// 게스트에서 서버로 보내는 패킷
		while(true) {
			retval = recv(socket, (char*)&packet, sizeof(packet), 0);
			if(retval > 0) {
				eventQue->toServer.Push(packet);
			}
			if(retval < 0) {
				break;
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
	packet.header.clientID = id;
	for(auto& guest : m_guestInfos) {
		guest.eventQue->toClient.Push(packet);
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

	DWORD optval = TIMEOUT;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
	optval = TRUE;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

	thread senderThread = thread{ &Guest::Sender, this };
	thread receiverThread = thread{ &Guest::Receiver, this };
	senderThread.detach();
	receiverThread.detach();
}

void Guest::Sender()
{
	int retval;
	Packet packet;

	while(GetState() == NETWORK_STATE::GUEST) {
		while(m_toServerEventQue.TryPop(packet)) {
			retval = send(m_socket, (char*)&packet, sizeof(Packet), 0);
			if(retval == SOCKET_ERROR) {
				// disconnect
				OutputDebugString(L"Disconnect\n");
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

		while(true) {
			retval = recv(m_socket, (char*)&packet, sizeof(Packet), 0);
			if(retval > 0) {
				m_receivedPacketQue.Push(packet);
			}
			if(retval < 0) {
				break;
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
	packet.header.clientID = id;
	m_toServerEventQue.Push(packet);
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
	m_network = make_unique<Host>();
}

void NetworkManager::Update()
{
	m_network->Update();
}

void NetworkManager::RunMulti()
{
	if(GetNetworkState() == NETWORK_STATE::SINGLE) {
		SetNetworkState(NETWORK_STATE::HOST);

		dynamic_cast<Host*>(m_network.get())->RunMulti();
	}
}

void NetworkManager::ConnectAsGuest()
{
	if(GetNetworkState() != NETWORK_STATE::GUEST) {

		dynamic_cast<Host*>(m_network.get())->Stop();

		m_network.release();
		m_network = make_unique<Guest>();

		SetNetworkState(NETWORK_STATE::GUEST);
		dynamic_cast<Guest*>(m_network.get())->Connect();
	}
}

void NetworkScript::LateUpdate()
{
	if(INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		GET_SINGLE(NetworkManager)->RunMulti();
		m_id = 0;
	}

	if(INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
		GET_SINGLE(NetworkManager)->m_id = 1;
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
		m_id = 1;
	}

	if(INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		Packet packet;
		packet.header.type = PACKET_TYPE::NET;
		packet.id = -1;
		GET_SINGLE(NetworkManager)->Send(packet);
	}
}
#pragma endregion