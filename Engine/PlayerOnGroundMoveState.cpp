#include "pch.h"
#include "PlayerOnGroundMoveState.h"
#include "PlayerOnGroundAimState.h"
#include "PlayerOnAirState.h"
#include "Player.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "Input.h"

shared_ptr<PlayerState> PlayerOnGroundMoveState::OnUpdateState()
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

	float forceMag = 300000;


	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);


	return nullptr;

}

void PlayerWalkState::OnEnter()
{
	float maxSpeed = m_player->GetRigidBody()->GetMaxSpeed();
	float speed = m_player->GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed > pow(maxSpeed * 0.6, 2))
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::RUN))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::RUN));
	}
	else if (speed > pow(maxSpeed * 0.01, 2))
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::WALK))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::WALK));
	}
	else
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::IDLE))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::IDLE));
	}
}

shared_ptr<PlayerState> PlayerWalkState::OnUpdateState()
{
	PlayerOnGroundMoveState::OnUpdateState();
	if (INPUT->GetButton(KEY_TYPE::RBUTTON))
	{
		return make_shared<PlayerIdleToAimState>(m_player);
	}
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		if (m_player->GetFireReady())
		{
			return make_shared<PlayerFireOnGroundMoveState>(m_player);
		}
	}
	return nullptr;
}

shared_ptr<PlayerState> PlayerWalkState::OnLateUpdateState()
{
	float maxSpeed = m_player->GetRigidBody()->GetMaxSpeed();
	float speed = m_player->GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed > pow(maxSpeed * 0.6, 2))
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::RUN))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::RUN));
	}
	else if (speed > pow(maxSpeed * 0.01, 2))
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::WALK))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::WALK));
	}
	else
	{
		if (m_player->GetAnimator()->GetCurrentClipIndex() != static_cast<uint32>(PLAYER_STATE::IDLE))
			m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::IDLE));
	}

	return nullptr;
}

void PlayerFireOnGroundMoveState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::FIRE));
	m_player->Fire();
}

shared_ptr<PlayerState> PlayerFireOnGroundMoveState::OnUpdateState()
{
	PlayerOnGroundMoveState::OnUpdateState();
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && m_player->GetFireReady())
	{
		m_isFireAnimationAgain = true;
	}
	return nullptr;
}

shared_ptr<PlayerState> PlayerFireOnGroundMoveState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame() || m_isFireAnimationAgain)
	{
		return make_shared<PlayerWaitFireOnGroundMoveState>(m_player);
	}
	return nullptr;
}

void PlayerWaitFireOnGroundMoveState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::AIM));
}

shared_ptr<PlayerState> PlayerWaitFireOnGroundMoveState::OnUpdateState()
{
	PlayerOnGroundMoveState::OnUpdateState();
	if (!INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		return make_shared<PlayerWalkState>(m_player);
	}
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && m_player->GetFireReady())
	{
		return make_shared<PlayerFireOnGroundMoveState>(m_player);
	}
	return nullptr;
}

shared_ptr<PlayerState> PlayerWaitFireOnGroundMoveState::OnLateUpdateState()
{
	return shared_ptr<PlayerState>();
}
