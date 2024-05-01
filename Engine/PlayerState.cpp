#include "pch.h"
#include "PlayerState.h"
#include "Player.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "Input.h"

shared_ptr<PlayerState> PlayerOnGroundState::OnUpdateState()
{
	shared_ptr<RigidBody> rb = m_player->GetRigidBody();
	shared_ptr<Transform> transform = m_player->GetTransform();
	shared_ptr<Animator> animator = m_player->GetAnimator();

	Vec3 forceDir = { 0,0,0 };

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		forceDir += transform->GetLook();
	}

	if (INPUT->GetButton(KEY_TYPE::S))
	{
		forceDir -= transform->GetLook();
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		forceDir -= transform->GetRight();
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		forceDir += transform->GetRight();
	}
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		rb->SetStatic(false);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		return make_shared<PlayerIdleToAimState>(m_player);
	}
	if (INPUT->GetButtonUp(KEY_TYPE::RBUTTON))
	{
		return make_shared<PlayerAimToIdleState>(m_player);
	}

	float forceMag = 300000;


	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);


	return nullptr;

}

void PlayerIdleState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::IDLE));
}

shared_ptr<PlayerState> PlayerIdleState::OnLateUpdateState()
{
	float maxSpeed = m_player->GetRigidBody()->GetMaxSpeed();
	float speed = m_player->GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed > pow(maxSpeed * 0.6, 2))
	{
		return make_shared<PlayerRunState>(m_player);
	}
	else if (speed > pow(maxSpeed * 0.01, 2))
	{
		return make_shared<PlayerWalkState>(m_player);
	}
	else
	{
		return nullptr;
	}
}

void PlayerRunState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::RUN));
}

shared_ptr<PlayerState> PlayerRunState::OnLateUpdateState()
{
	float maxSpeed = m_player->GetRigidBody()->GetMaxSpeed();
	float speed = m_player->GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed < pow(maxSpeed * 0.01, 2))
	{
		return make_shared<PlayerIdleState>(m_player);
	}
	else if (speed < pow(maxSpeed * 0.6, 2))
	{
		return make_shared<PlayerWalkState>(m_player);
	}
	else
	{
		return nullptr;
	}
}

void PlayerWalkState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::WALK));
}

shared_ptr<PlayerState> PlayerWalkState::OnLateUpdateState()
{
	float maxSpeed = m_player->GetRigidBody()->GetMaxSpeed();
	float speed = m_player->GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed < pow(maxSpeed * 0.01, 2))
	{
		return make_shared<PlayerIdleState>(m_player);
	}
	else if (speed > pow(maxSpeed * 0.6, 2))
	{
		return make_shared<PlayerRunState>(m_player);
	}
	else
	{
		return nullptr;
	}
}

void PlayerAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::AIM));
}

void PlayerIdleToAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::IDLE_TO_AIM));
}

shared_ptr<PlayerState> PlayerIdleToAimState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsFirstLapCompleted())
	{
		return make_shared<PlayerAimState>(m_player);
	}
	return nullptr;
}

void PlayerAimToIdleState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::AIM_TO_IDLE));
}

shared_ptr<PlayerState> PlayerAimToIdleState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsFirstLapCompleted())
	{
		return make_shared<PlayerIdleState>(m_player);
	}
	return nullptr;
}