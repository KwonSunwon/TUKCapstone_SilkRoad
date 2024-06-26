#include "pch.h"
#include "Enemy.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "EnemyState.h"
#include "Timer.h"
#include "Network.h"
#include "Packet.h"
#include "Animator.h"

void Enemy::Awake()
{
	SetMonovihaviourName("Enemy");

	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
	rb->SetMaxSpeed(m_maxWalkSpeed);
	rb->SetMass(80.f);

	m_curState = make_shared<EnemyIdleState>(shared_from_this());

}
void Enemy::Update()
{
	if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::GUEST) {
		return;
	}

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if(m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;


	shared_ptr<EnemyState> nextState = m_curState->OnUpdateState();
	if(nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::LateUpdate()
{
	if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::GUEST) {
		return;
	}
	else if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST) {
		shared_ptr<EnemyPacket> packet = make_shared<EnemyPacket>();
		packet->m_targetId = m_networkId;
		packet->m_position = GetTransform()->GetLocalPosition();
		packet->m_velocity = GetRigidBody()->GetLinearVelocity();
		packet->m_rotation = GetTransform()->GetLocalRotation();
		packet->m_targetPlayerId = GetTargetPlayerIndex();
		packet->m_animationIndex = GetAnimator()->GetCurrentClipIndex();
		SEND(packet);
	}

	shared_ptr<EnemyState> nextState = m_curState->OnLateUpdateState();
	if(nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::ProcessPacket(shared_ptr<EnemyPacket> packet)
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	transform->SetLocalRotation(packet->m_rotation);
	SetTargetPlayerIndex(packet->m_targetPlayerId);
	if(GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
}

void Enemy::Fire()
{
	m_fireElapsedTime = 1.f / m_fireRate;
}
