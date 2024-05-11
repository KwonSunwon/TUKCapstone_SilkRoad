#include "pch.h"
#include "Player.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "PlayerOnGroundMoveState.h"
#include "PlayerOnGroundAImState.h"
#include "PlayerOnAirState.h"
#include "PlayerBullet.h"
#include "Network.h"
#include "Packet.h"
#include "Animator.h"
#include "Manifold.h"
#include "Item.h"

void Player::Awake()
{
	SetMonovihaviourName("Player");
	shared_ptr<RigidBody> rb = GetRigidBody();
	//rb->SetStatic(true);
	m_curState = make_shared<PlayerWalkState>(shared_from_this());

}
void Player::Update()
{
	ProcessGetItem();

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if (m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;

	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement

	rot.y += mouseDelta.x * (double)0.001;
	if (rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;


	GetTransform()->SetLocalRotation(rot);


	shared_ptr<PlayerState> nextState = m_curState->OnUpdateState();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Player::LateUpdate()
{
	shared_ptr<PlayerState> nextState = m_curState->OnLateUpdateState();
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}

	shared_ptr<PlayerPacket> playerPacket = make_shared<PlayerPacket>();
	playerPacket->m_targetId = GET_SINGLE(NetworkManager)->m_networkId;
	playerPacket->m_position = rb->GetPosition();
	playerPacket->m_velocity = rb->GetLinearVelocity();
	playerPacket->m_rotation = transform->GetLocalRotation();
	playerPacket->m_animationIndex = GetAnimator()->GetCurrentClipIndex();
	SEND(playerPacket)
}

void Player::Fire()
{
	m_fireElapsedTime = 1.f / m_fireRate;

	BulletType bulletType = CalcBulletType();
	m_bullets[m_bulletPivot++]->Fire(shared_from_this(), bulletType);
	m_fireTime++;


	if (m_bulletPivot >= m_bullets.size())
		m_bulletPivot = 0;
}

void Player::AddBullet(shared_ptr<class PlayerBullet> bullet)
{
	m_bullets.push_back(bullet);
}

void Player::ProcessGetItem()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptI = col->m_rb2->GetGameObject()->GetMonobehaviour("Item");
		if (scriptI) {
			shared_ptr<Item> itemScript = dynamic_pointer_cast<Item>(scriptI);
			m_itemLevels[itemScript->GetItemID()]++;
			col->m_rb2->MoveTo(Vec3(0, 1000000, 0));
		}
	}
}

BulletType Player::CalcBulletType()
{
	if (m_itemLevels[0] == 0)
		return BulletType::BASIC;

	if (m_fireTime % (5 - m_itemLevels[0]) == 0)
		return BulletType::EXPLOSIVE;

	return BulletType::BASIC;
}
