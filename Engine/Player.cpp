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

void Player::Awake()
{
	SetMonovihaviourName("Player");
	shared_ptr<RigidBody> rb = GetRigidBody();
	//rb->SetStatic(true);
	m_curState = make_shared<PlayerWalkState>(shared_from_this());

}
void Player::Update()
{

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
	m_bullets[m_bulletPivot++]->Fire(shared_from_this(),BulletType::BASIC);
	if (m_bulletPivot >= m_bullets.size())
		m_bulletPivot = 0;
}

void Player::AddBullet(shared_ptr<class PlayerBullet> bullet)
{
	m_bullets.push_back(bullet);
}
