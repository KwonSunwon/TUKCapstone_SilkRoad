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
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
		throw runtime_error("Fail initialize WSADATA");
	}
}

void Network::Update()
{
	shared_ptr<Packet> packet = make_shared<Packet>();
	auto objects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();
	auto players = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers();

	while (Recv(packet)) {
		switch (packet->header.type) {
		case PACKET_TYPE::PLAYER:
			players[packet->id]->GetTransform()->SetLocalPosition(packet->pos);
			break;
		case PACKET_TYPE::ENEMY:
			// find target object by id
			for (auto& object : objects) {
				if (object->GetID() == packet->id) {
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

	//m_gameData[0].pos = Vec3{ 0, 0, 0 };
	//m_gameData[0].id = 0;
	//m_gameData[1].pos = Vec3{ 0, 0, 0 };
	//m_gameData[1].id = 1;
	//m_lastGameData[0].pos = Vec3{ 0, 0, 0 };
	//m_lastGameData[0].id = 0;
	//m_lastGameData[1].pos = Vec3{ 0, 0, 0 };
	//m_lastGameData[1].id = 1;
}

void Host::MainLoop()
{
	while (GetState() != NETWORK_STATE::GUEST) {
		{
			Packet packet;
			if (GetState() == NETWORK_STATE::HOST) {
				// �Խ�Ʈ�� ���� ��Ŷ�� �޾� ����
				for (auto& guest : m_guestInfos) {
					while (guest.eventQue->toServer.TryPop(packet))
						m_gameLoopEventQue.push(packet);
				}
			}
			// ȣ��Ʈ Ŭ���̾�Ʈ�� Ǫ���� ��Ŷ�� �޾� ����
			string str = "MainLoop: Queue Size - " + to_string(m_eventQue.toServer.Size()) + "\n";
			while (m_eventQue.toServer.TryPop(packet)) {
				m_gameLoopEventQue.push(packet);
			}
		}
		GameLoop();
		{
			m_timer += DELTA_TIME;

			if (m_timer > SEND_PACKET_PER_SEC) {
				while (!m_outGameLoopEventQue.empty()) {
					Packet packet = m_outGameLoopEventQue.front();
					m_outGameLoopEventQue.pop();
					for (int i = 0; i < MAX_PLAYER; i++) {
						for (auto& guest : m_guestInfos) {
							guest.eventQue->toClient.Push(packet);
						}
						m_eventQue.toClient.Push(packet);
					}
				}
				m_timer = 0.0f;
			}
		}
	}
	OutputDebugString(L"Host MainLoop End\n");
}

void Host::GameLoop()
{
	while (!m_gameLoopEventQue.empty()) {
		Packet packet = m_gameLoopEventQue.front();
		m_gameLoopEventQue.pop();

		switch (packet.header.type) {
		case PACKET_TYPE::PLAYER:
			m_gameData.playerData[packet.id].pos = packet.pos;
			// ... TEMP: �����ϰ� �÷��̾� ��ġ ����
			// ���� �ڵ�� �� ������ ���� ����� ���
			m_outGameLoopEventQue.push(packet);
			break;
		case PACKET_TYPE::ENEMY:
			break;
			// ...
		case PACKET_TYPE::NET:
			// guest disconnect
			break;
		default:
			break;
		}
	}

	// TODO: Server game logic
	// Enemy AI, Collision, etc...
}

void Host::Update()
{
	int count = 0;
	shared_ptr<Packet> packet = make_shared<Packet>();
	while (Recv(packet)) {
		//// ���ӿ� ����
		//if (packet.id == 0) {
		//	string str = "Update: Packet " + to_string(count++) + " - " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
		//	OutputDebugStringA(str.c_str());
		//}
		//if (packet.id == 1) {
		//	string str2 = "Update: Packet2 " + to_string(count++) + " - " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
		//	OutputDebugStringA(str2.c_str());
		//}

		/*auto players = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers();
		for (auto& player : players) {
			if (player)
				if (player->GetID() == packet.id) {
					player->GetTransform()->SetLocalPosition(packet.pos);
					break;
				}
		}*/

		GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers()[packet->id]->GetTransform()->SetLocalPosition(packet->pos);
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

	if (listen(listenSocket, MAX_PLAYER) == SOCKET_ERROR) {
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
	m_guestInfos.reserve(MAX_PLAYER);
	int playerCount = 0;
	while (GetState() == NETWORK_STATE::HOST) {
		// WaitForClients
		addrLen = sizeof(clientAddr);
		SOCKET tempSocket = ::accept(m_listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (tempSocket == INVALID_SOCKET) {
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
	for (auto& guest : m_guestInfos) {
		if (guest.id == id) {
			socket = guest.socket;
			eventQue = guest.eventQue;
			break;
		}
	}

	// �Խ�Ʈ�� ��Ŷ �ۼ���
	int retval;
	while (GetState() == NETWORK_STATE::HOST) {
		Packet packet;
		// �������� �Խ�Ʈ�� ������ ��Ŷ
		while (eventQue->toClient.TryPop(packet)) {
			retval = send(socket, (char*)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR) {
				// disconnect
				OutputDebugString(L"Disconnect\n");
				closesocket(socket);
				break;
			}
		}
		// �Խ�Ʈ���� ������ ������ ��Ŷ
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
bool Host::Recv(shared_ptr<Packet> packet)
{
	if (m_eventQue.toClient.TryPop(*packet.get()))
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

	DWORD optval = TIMEOUT;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
	optval = TRUE;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));
}

void Guest::Update()
{
	// ��Ŷ�� �ް� ���ӿ� �����ϴ� �κ�
	int count = 0;
	int retval;
	shared_ptr<Packet> packet = make_shared<Packet>();
	while (Recv(packet)) {
		//string str = "Update: Packet " + to_string(count++) + " - " + to_string(packet->pos.x) + ", " + to_string(packet->pos.y) + ", " + to_string(packet->pos.z) + "\n";
		//OutputDebugStringA(str.c_str());

		/*auto players = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers();
		for (auto& player : players) {
			if (player)
				if (player->GetID() == packet->id) {
					player->GetTransform()->SetLocalPosition(packet->pos);
					break;
				}
		}*/

		GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers()[packet->id]->GetTransform()->SetLocalPosition(packet->pos);
	}
}

void Guest::Send(Packet packet)
{
	int retval = send(m_socket, (char*)&packet, sizeof(Packet), 0);
	if (retval == SOCKET_ERROR) {
		// disconnect
		OutputDebugString(L"Disconnect\n");
		closesocket(m_socket);

		// TODO: ���� ���� �� ó��
		// ��õ� �� ���� ���� ��
		// �ٽ� �̱۷� ��ȯ
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
	m_network->Update();
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
		// ȣ��Ʈ���� �Խ�Ʈ�� ��ȯ
		GET_SINGLE(NetworkManager)->m_id = 1;
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
		m_id = 1;
		//GET_SINGLE(SceneManager)->AddPlayer(m_id);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_3)) {
		// �Խ�Ʈ���� �̱۷� ��ȯ
		Packet packet;
		packet.header.type = PACKET_TYPE::NET;
		packet.id = -1;
		GET_SINGLE(NetworkManager)->Send(packet);
	}

	//Packet packet;
	//shared_ptr<GameObject> player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers()[m_id];
	//packet.pos = player->GetTransform()->GetLocalPosition();
	//packet.id = m_id;
	////string str = "Send(" + to_string(m_id) + "): " + to_string(packet.pos.x) + ", " + to_string(packet.pos.y) + ", " + to_string(packet.pos.z) + "\n";
	////OutputDebugStringA(str.c_str());
	//GET_SINGLE(NetworkManager)->Send(packet);
}
#pragma endregion