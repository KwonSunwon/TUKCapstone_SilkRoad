#include "Pch.h"

#include "Network.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "Enemy.h"
#include "NetworkObject.h"
#include "NetworkPlayer.h"
#include "Player.h"
#include "UpgradeManager.h"
#include "StagePortal.h"

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
	auto objects = GET_SINGLE(SceneManager)->GetActiveScene()->GetNetworkObjects();
	auto players = GET_SINGLE(SceneManager)->GetActiveScene()->GetNetworkPlayers();

	shared_ptr<Packet> packet;
	int id;
	//while(Recv(m_packetBuffer)) {
	while(m_receivedPacketQue.TryPop(packet)) {
		switch(packet->m_type) {
		case PACKET_TYPE::PT_NONE:
			break;
		case PACKET_TYPE::PT_GUEST_INIT:
			id = ProcessPlayerID(packet->m_targetId);
			if(id != -1) {
				players[id]->ChangeClass(reinterpret_pointer_cast<GuestInitPacket>(packet)->m_classIndex);
				players[id]->SetActivated(true);
			}
			break;
		case PACKET_TYPE::PT_PLAYER:
			id = ProcessPlayerID(packet->m_targetId);
			if(id != -1)
				players[id]->ProcessPacket(reinterpret_pointer_cast<PlayerPacket>(packet));
			break;
		case PACKET_TYPE::PT_PLAYER_CLASS_CHANGE:
			id = ProcessPlayerID(packet->m_targetId);
			if(id != -1) {
				players[id]->ChangeClass(reinterpret_pointer_cast<PlayerClassChangePacket>(packet)->m_classIndex);
				players[id]->SetActivated(true);
			}
			break;
		case PACKET_TYPE::PT_PLAYER_HIT:
			GET_SINGLE(SceneManager)->GetActiveScene()->m_mainPlayerScript->ApplyDamage(reinterpret_pointer_cast<PlayerHitPacket>(packet)->m_damage);
			break;
		case PACKET_TYPE::PT_SKILL:
			GET_SINGLE(SceneManager)->GetActiveScene()->m_mainPlayerScript->NetworkSkill(reinterpret_pointer_cast<SkillPacket>(packet));
			break;
		case PACKET_TYPE::PT_ENEMY:
		case PACKET_TYPE::PT_ENEMY_HIT:
		case PACKET_TYPE::PT_ITEM:
		case PACKET_TYPE::PT_FORCE:
			objects[packet->m_targetId]->ProcessPacket(packet);
			break;
		case PACKET_TYPE::PT_STAGE_CHANGE:
			if(m_networkState == NETWORK_STATE::GUEST) {
				m_receivedPacketQue.Clear();
				GET_SINGLE(SceneManager)->StartNextStage();
			}
			break;
		case PACKET_TYPE::PT_PARTICLE: {
			auto pp = reinterpret_pointer_cast<ParticlePacket>(packet);
			if(pp->m_particleIndex == ParticleType::PARTICLE_PORTAL)
				break;
			GET_SINGLE(SceneManager)->GetActiveScene()->SpawnParticle(reinterpret_pointer_cast<ParticlePacket>(packet)->m_pos, reinterpret_pointer_cast<ParticlePacket>(packet)->m_particleIndex, true);
			break;
		}
		case PACKET_TYPE::PT_PORTAL_ON: {
			auto portal = GET_SINGLE(SceneManager)->GetActiveScene()->GetStagePortal();
			portal->m_isPacketProcess = true;
			portal->InteractiveFunction();
			break;
		}
		default:
			break;
		}
	}
}

shared_ptr<Packet> Network::PacketProcess(int idx)
{
	PACKET_TYPE packetType = static_cast<PACKET_TYPE>(m_buffer[idx].Peek(2));
	shared_ptr<Packet> packet = nullptr;
	switch(packetType) {
	case PACKET_TYPE::PT_NONE:
		return nullptr;
	case PACKET_TYPE::PT_INIT:
		packet = make_shared<InitPacket>();
		break;
	case PACKET_TYPE::PT_PLAYER:
		packet = make_shared<PlayerPacket>();
		break;
	case PACKET_TYPE::PT_ENEMY:
		packet = make_shared<EnemyPacket>();
		break;
	case PACKET_TYPE::PT_ENEMY_HIT:
		packet = make_shared<EnemyHitPacket>();
		break;
	case PACKET_TYPE::PT_STAGE_CHANGE:
		packet = make_shared<StageChangePacket>();
		break;
	case PACKET_TYPE::PT_PLAYER_CLASS_CHANGE:
		packet = make_shared<PlayerClassChangePacket>();
		break;
	case PACKET_TYPE::PT_SKILL:
		packet = make_shared<SkillPacket>();
		break;
	case PACKET_TYPE::PT_ITEM:
		packet = make_shared<ItemPacket>();
		break;
	case PACKET_TYPE::PT_GUEST_INIT:
		packet = make_shared<GuestInitPacket>();
		break;
	case PACKET_TYPE::PT_PARTICLE:
		packet = make_shared<ParticlePacket>();
		break;
	case PACKET_TYPE::PT_FORCE:
		packet = make_shared<ForcePacket>();
		break;
	case PACKET_TYPE::PT_PLAYER_HIT:
		packet = make_shared<PlayerHitPacket>();
		break;
	case PACKET_TYPE::PT_PORTAL_ON:
		packet = make_shared<PortalOnPacket>();
		break;
	}

	if(!m_buffer[idx].Read(reinterpret_cast<char*>(packet.get()), packet->m_size)) {
		OutputDebugString(L"(PacketProcess)Fail read buffer - required size > m_size\n");
		return nullptr;
	}

	return packet;
}

int Network::ProcessPlayerID(int id)
{
	if(id != 0 && id != 1 && id != 2) {
		OutputDebugString(L"Invalid PlayerPacket targetId\n");
		return -1;
	}
	if(m_networkState == NETWORK_STATE::HOST && id == 1)
		return 0;
	if(id == 2)
		return 1;
	return id;
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
		initPacket.m_classIndex = GET_SINGLE(UpgradeManager)->GetClass();
		if(playerCount == 2)
			initPacket.m_classIndexGuest = GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->GetClassIdx();
		else
			initPacket.m_classIndexGuest = -1;
		send(guest.socket, (char*)&initPacket, sizeof(InitPacket), 0);

		DWORD optval = TIMEOUT;
		setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
		/*optval = TRUE;
		setsockopt(tempSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));*/

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

	int bufferIdx = id == 1 ? 0 : 1;

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

		int otherId = id == 1 ? 1 : 0;
		// Write received data to buffer
		retval = recv(socket, buffer.get(), BUFFER_SIZE - m_buffer[bufferIdx].Size(), 0);
		if(retval > 0) {
			m_buffer[bufferIdx].Write(buffer.get(), retval);

			ushort packetSize = m_buffer[bufferIdx].Peek();
			while(packetSize <= m_buffer[bufferIdx].Size()) {
				if(packetSize == -1)
					break;

				packet = PacketProcess(bufferIdx);
				if(packet == nullptr) {
					OutputDebugString(L"Fail PacketProcess\n");
					break;
				}

				if(m_guestInfos.size() > 1 && IsThroughPacket(packet->m_type)) {
					send(m_guestInfos[otherId].socket, (char*)packet.get(), packet->m_size, 0);
				}
				m_receivedPacketQue.Push(packet);

				packetSize = 0;
				packet.reset();

				if(m_buffer[bufferIdx].Empty())
					break;

				packetSize = m_buffer[bufferIdx].Peek();
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

void Host::Send(shared_ptr<char[]> data, int size, int guestId)
{
	if(m_guestInfos.empty())
		return;
	send(m_guestInfos[guestId].socket, data.get(), size, 0);
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
bool Host::IsThroughPacket(PACKET_TYPE type)
{
	switch(type) {
	case PACKET_TYPE::PT_PLAYER:
	case PACKET_TYPE::PT_PLAYER_CLASS_CHANGE:
	case PACKET_TYPE::PT_ENEMY_HIT:
	case PACKET_TYPE::PT_SKILL:
	case PACKET_TYPE::PT_ITEM:
	case PACKET_TYPE::PT_GUEST_INIT:
	case PACKET_TYPE::PT_PARTICLE:
	case PACKET_TYPE::PT_FORCE:
		return true;
	}
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
	OutputDebugString(L"Retval: ");
	OutputDebugString(to_wstring(retval).c_str());
	if(retval == SOCKET_ERROR) {
		//err_display(retval);
		throw runtime_error("Fail connect server");
	}

	InitPacket initPacket;
	recv(m_socket, (char*)&initPacket, sizeof(InitPacket), 0);
	GET_SINGLE(NetworkManager)->m_networkId = initPacket.m_networkId;
	GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->ChangeClass(initPacket.m_classIndex);
	GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[0]->SetActivated(true);
	if(initPacket.m_classIndexGuest != -1) {
		GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[1]->ChangeClass(initPacket.m_classIndexGuest);
		GET_SINGLE(SceneManager)->GetActiveScene()->m_networkPlayers[1]->SetActivated(true);
	}

	OutputDebugString(L"\nNetworkId: ");
	OutputDebugString(to_wstring(GET_SINGLE(NetworkManager)->m_networkId).c_str());
	OutputDebugString(L"\n");

	DWORD optval = TIMEOUT;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
	/*optval = TRUE;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));*/

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

	shared_ptr<GuestInitPacket> packet = make_shared<GuestInitPacket>();
	packet->m_targetId = initPacket.m_networkId;
	packet->m_classIndex = GET_SINGLE(UpgradeManager)->GetClass();
	SEND(packet);
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
		retval = recv(m_socket, buffer.get(), BUFFER_SIZE - m_buffer[0].Size(), 0);
		if(retval > 0) {
			m_buffer[0].Write(buffer.get(), retval);

			ushort packetSize = m_buffer[0].Peek();
			while(packetSize <= m_buffer[0].Size()) {
				if(packetSize == -1)
					break;

				packet = PacketProcess(0);
				if(packet == nullptr) {
					OutputDebugString(L"Fail PacketProcess\n");
					break;
				}
				m_receivedPacketQue.Push(packet);

				packetSize = 0;
				packet.reset();

				if(m_buffer[0].Empty())
					break;

				packetSize = m_buffer[0].Peek();
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

	m_prevTime = chrono::system_clock::now();
	m_remainTime = SEND_PACKET_PER_SEC;
}

void NetworkManager::Update()
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Update();

	m_isSend = false;

	auto elapsedTime = chrono::duration<double>(chrono::system_clock::now() - m_prevTime).count();
	m_remainTime -= elapsedTime;
	if(m_remainTime <= 0) {
		m_prevTime = chrono::system_clock::now();
		m_remainTime = SEND_PACKET_PER_SEC;
		m_isSend = true;
	}
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

void NetworkManager::Send(shared_ptr<char[]> data, int size, int guestId)
{
	if(GetNetworkState() != NETWORK_STATE::SINGLE)
		m_network->Send(data, size, guestId);
}

bool NetworkManager::Recv(shared_ptr<Packet> packet)
{
	if(GetNetworkState() == NETWORK_STATE::SINGLE)
		return false;
	return m_network->Recv(packet);
}

void NetworkScript::LateUpdate()
{
	/*if(INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		GET_SINGLE(NetworkManager)->m_networkId = 0;
		GET_SINGLE(NetworkManager)->RunMulti();
	}*/

	/*if(INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
	}*/

	if(INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		/*Packet packet;
		packet.header.type = PACKET_TYPE::NET;
		packet.id = -1;
		GET_SINGLE(NetworkManager)->Send(packet);*/
	}
}
#pragma endregion