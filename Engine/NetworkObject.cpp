#include "pch.h"
#include "Packet.h"
#include "NetworkObject.h"
#include "MonoBehaviour.h"
#include "Network.h"
#include "GameObject.h"
#include "Enemy.h"
#include "RigidBody.h"
#include "Item.h"

NetworkObject::NetworkObject() : Component(COMPONENT_TYPE::NETWORK)
{

}

NetworkObject::~NetworkObject()
{

}

void NetworkObject::ProcessPacket(shared_ptr<Packet> packet)
{
	shared_ptr<MonoBehaviour> scriptE = GetGameObject()->GetMonobehaviour("Enemy");
	shared_ptr<MonoBehaviour> scriptI = GetGameObject()->GetMonobehaviour("Item");

	switch(packet->m_type) {
	case PACKET_TYPE::PT_ENEMY:
	case PACKET_TYPE::PT_ENEMY_HIT: {
		shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(scriptE);
		if(packet->m_type == PACKET_TYPE::PT_ENEMY)
			enemy->ProcessPacket(reinterpret_pointer_cast<EnemyPacket>(packet));
		else {
			enemy->GetDamage(reinterpret_pointer_cast<EnemyHitPacket>(packet)->m_damage, true);
			if(!enemy->GetRigidBody()->GetStatic())
				enemy->GetRigidBody()->AddForce(reinterpret_pointer_cast<EnemyHitPacket>(packet)->m_rayDir * reinterpret_pointer_cast<EnemyHitPacket>(packet)->m_knockBackPower * 1000000.f);
		}
		break;
	}
	case PACKET_TYPE::PT_ITEM: {
		if(!scriptI) {
			OutputDebugString(L"Item Script is Null - NetworkID: ");
			OutputDebugString(to_wstring(GetNetworkId()).c_str());
			OutputDebugString(L"\n");
			return;
		}
		shared_ptr<Item> item = dynamic_pointer_cast<Item>(scriptI);
		item->GetRigidBody()->MoveTo(reinterpret_pointer_cast<ItemPacket>(packet)->m_pos);
		break;
	}
	}
}