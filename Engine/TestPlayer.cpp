#include "pch.h"
#include "TestPlayer.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Terrain.h"
#include "Network.h"
#include "Packet.h"

TestPlayer::TestPlayer(int id) : m_id(id)
{
}

void TestPlayer::LateUpdate()
{
	if(m_id != GET_SINGLE(NetworkManager)->m_networkId)
		return;

	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 prev = pos;

	if(GET_SINGLE(Input)->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * 5.f * DELTA_TIME;
	if(GET_SINGLE(Input)->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * 5.f * DELTA_TIME;

	if(GET_SINGLE(Input)->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * 5.f * DELTA_TIME;
	if(GET_SINGLE(Input)->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * 5.f * DELTA_TIME;

	GetTransform()->SetLocalPosition(pos);

	//if(prev != pos && GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
	if(prev != pos)
		if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
			MovePacket packet;
			packet.m_targetId = m_id;
			packet.m_position = pos;
			SOCKET socket = GET_SINGLE(NetworkManager)->m_network->m_socket;
			send(socket, (char*)&packet, sizeof(MovePacket), 0);
		}
}
