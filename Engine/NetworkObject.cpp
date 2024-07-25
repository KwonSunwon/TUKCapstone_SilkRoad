#include "pch.h"
#include "Packet.h"
#include "NetworkObject.h"
#include "MonoBehaviour.h"
#include "Network.h"
#include "GameObject.h"
#include "Enemy.h"

NetworkObject::NetworkObject() : Component(COMPONENT_TYPE::NETWORK)
{

}

NetworkObject::~NetworkObject()
{

}

void NetworkObject::ProcessPacket(shared_ptr<Packet> packet)
{
	shared_ptr<MonoBehaviour> scriptE = GetGameObject()->GetMonobehaviour("Enemy");

	switch(packet->m_type)
	{
	case PACKET_TYPE::PT_ENEMY:
	{
		shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(scriptE);
		enemy->ProcessPacket(reinterpret_pointer_cast<EnemyPacket>(packet));
		break;
	}
	default:
		break;
	}
}