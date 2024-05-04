#include "pch.h"
#include "EnemyState.h"
#include "Player.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "Input.h"
#include "Enemy.h"

shared_ptr<EnemyState> EnemyIdleState::OnUpdateState()
{
	for (int i = 0; i < m_enemy->GetPlayers().size(); ++i)
	{
		shared_ptr<GameObject> player = m_enemy->GetPlayers()[i];
		Vec3 toPlayer = player->GetTransform()->GetLocalPosition() - m_enemy->GetTransform()->GetLocalPosition();
		float dist = toPlayer.Length();
		if (dist < m_enemy->GetChaseRange())
		{
			m_enemy->SetTargetPlayerIndex(i);
			return make_shared<EnemyWalkState>(m_enemy);
		}

	}
	return nullptr;
}

void EnemyIdleState::OnEnter()
{
	m_enemy->GetAnimator()->Play(static_cast<uint32>(ENEMY_STATE::IDLE));
}

shared_ptr<EnemyState> EnemyWalkState::OnUpdateState()
{
	shared_ptr<GameObject> player = m_enemy->GetPlayers()[m_enemy->GetTargetPlayerIndex()];
	Vec3 toPlayer = player->GetTransform()->GetLocalPosition() - m_enemy->GetTransform()->GetLocalPosition();
	float dist = toPlayer.Length();
	if (dist < m_enemy->GetAttackRange())
	{
		if(m_enemy->GetAttackReady())
			return make_shared<EnemyAttackState>(m_enemy);
	}
	else if (dist > m_enemy->GetChaseRange())
	{
		return make_shared<EnemyIdleState>(m_enemy);
	}
	else
	{
		Vec3 dir = toPlayer;
		dir.y = 0;
		shared_ptr<RigidBody> rb = m_enemy->GetRigidBody();
		shared_ptr<Transform> transform = m_enemy->GetTransform();

		dir.Normalize();
		rb->SetMaxSpeed(m_enemy->GetMaxWalkSpeed());
		transform->LookAt(dir);

		Vec3 force = dir * m_enemy->GetWalkForce();
		rb->AddForce(force);
	}
	return nullptr;
}

void EnemyWalkState::OnEnter()
{
	m_enemy->GetAnimator()->Play(static_cast<uint32>(ENEMY_STATE::WALK));
	// 플레이어 타격
}

shared_ptr<EnemyState> EnemyAttackState::OnUpdateState()
{
	return nullptr;
}

shared_ptr<EnemyState> EnemyAttackState::OnLateUpdateState()
{
	if (m_enemy->GetAnimator()->IsAnimationEndOnThisFrame())
	{
		return make_shared<EnemyIdleState>(m_enemy);
	}
}

void EnemyAttackState::OnEnter()
{
	m_enemy->GetAnimator()->Play(static_cast<uint32>(ENEMY_STATE::ATTACK));
}
