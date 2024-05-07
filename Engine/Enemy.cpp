#include "pch.h"
#include "Enemy.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "EnemyState.h"
#include "Timer.h"

void Enemy::Awake()
{
	name = "Enemy";

	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
	rb->SetMaxSpeed(m_maxWalkSpeed);
	rb->SetMass(80.f);

	m_curState = make_shared<EnemyIdleState>(shared_from_this());

}
void Enemy::Update()
{
	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if (m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;


	shared_ptr<EnemyState> nextState = m_curState->OnUpdateState();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::LateUpdate()
{
	shared_ptr<EnemyState> nextState = m_curState->OnLateUpdateState();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::Fire()
{
	m_fireElapsedTime = 1.f / m_fireRate;
}
