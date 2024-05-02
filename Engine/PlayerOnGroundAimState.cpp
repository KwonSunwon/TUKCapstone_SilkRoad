#include "pch.h"
#include "PlayerOnGroundAimState.h"
#include "PlayerOnGroundMoveState.h"
#include "PlayerOnAirState.h"
#include "Player.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "Input.h"

shared_ptr<PlayerState> PlayerOnGroundAimState::OnUpdateState()
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

	float forceMag = 300000;


	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);


	return nullptr;

}

void PlayerAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::AIM));
}

shared_ptr<PlayerState> PlayerAimState::OnUpdateState()
{
	PlayerOnGroundAimState::OnUpdateState();
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && m_player->GetFireReady())
	{
		m_player->Fire();
		return make_shared<PlayerFireOnAimState>(m_player);
	}	
	if (!INPUT->GetButton(KEY_TYPE::RBUTTON))
	{
		return make_shared<PlayerAimToIdleState>(m_player);
	}
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

void PlayerFireOnAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::FIRE));
}

shared_ptr<PlayerState> PlayerFireOnAimState::OnUpdateState()
{
	PlayerOnGroundAimState::OnUpdateState();
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && INPUT->GetButton(KEY_TYPE::RBUTTON) && m_player->GetFireReady())
	{
		m_isFireAnimationAgain = true;
	}
	return nullptr;
}

shared_ptr<PlayerState> PlayerFireOnAimState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsFirstLapCompleted() || m_isFireAnimationAgain)
	{
		return make_shared<PlayerAimState>(m_player);
	}
	return nullptr;
}
