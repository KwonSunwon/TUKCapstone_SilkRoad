#include "pch.h"
#include "Item.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Timer.h"
#include "TextObject.h"
#include "UIObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Network.h"
#include "Packet.h"
#include "NetworkObject.h"

void Item::Awake()
{
	SetMonovihaviourName("Item");
	m_startHeight = GetTransform()->GetLocalPosition().y;


}
void Item::Update()
{
	FloatingItem();
	RotateItem();
}

void Item::LateUpdate()
{

}

void Item::FloatingItem()
{
	m_radian += m_frequency * DELTA_TIME * PI * 2;
	float nextHeight = m_startHeight + m_amplitude * sin(m_radian);
	Vec3 nextPos = GetRigidBody()->GetPosition();
	nextPos.y = nextHeight;
	GetRigidBody()->MoveTo(nextPos);
}
void Item::RotateItem()
{
	Vec3 rot = GetTransform()->GetLocalRotation();
	rot.y = m_radian;
	GetTransform()->SetLocalRotation(rot);
}

void Item::AddGetItemText()
{

	if(m_itemId == -1)
	{
		return;
	}

	auto text = make_shared<GettingItemTextObject>(L"Brilliant Behemoth\nAdds explosion to bullets.");

	switch(m_itemId)
	{
	case 0:
		text->SetText(L"Brilliant Behemoth\nAdds explosion to bullets.");
		break;
	case 1:
		text->SetText(L"Battery Pack\nIncrease Attack Speed.");
		break;
	case 2:
		text->SetText(L"Fat Bullet\nIncrease Bullet Damage.");
		break;
	case 3:
		text->SetText(L"Gunpowder Pack\nIncrease explosion Damage.");
		break;
	case 4:
		text->SetText(L"Ham\nIncrease Explosion Range.");
		break;
	}

	text->SetPivot(ePivot::CENTER);
	text->SetScreenPivot(ePivot::CENTER);
	float yOffset = text->getNum() * 90.f;
	text->SetPosition(Vec2(0.f, yOffset));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddTextObject(text);

	if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
		shared_ptr<ItemPacket> packet = make_shared<ItemPacket>();
		packet->m_targetId = GetNetworkObject()->GetNetworkId();
		packet->m_pos = Vec3(0, 1000000, 0);
		SEND(packet);
	}
}

void Item::ProcessPacket(shared_ptr<class ItemPacket> packet)
{
	GetRigidBody()->MoveTo(Vec3(0, 1'000'000, 0));
}
