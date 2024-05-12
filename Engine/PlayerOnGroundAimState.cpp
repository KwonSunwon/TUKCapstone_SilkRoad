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
#include "Manifold.h"

shared_ptr<PlayerState> PlayerOnGroundAimState::OnUpdateState()
{
	shared_ptr<RigidBody> rb = m_player->GetRigidBody();
	shared_ptr<Transform> transform = m_player->GetTransform();
	shared_ptr<Animator> animator = m_player->GetAnimator();

	Vec3 forceDir = { 0,0,0 };

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		Vec3 lookProj = Vec3(transform->GetLook().x, 0, transform->GetLook().z);
		lookProj.Normalize();
		forceDir += lookProj;
	}

	if (INPUT->GetButton(KEY_TYPE::S))
	{
		Vec3 lookProj = Vec3(transform->GetLook().x, 0, transform->GetLook().z);
		lookProj.Normalize();
		forceDir -= lookProj;
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		Vec3 rightProj = Vec3(transform->GetRight().x, 0, transform->GetRight().z);
		rightProj.Normalize();
		forceDir -= rightProj;
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		Vec3 rightProj = Vec3(transform->GetRight().x, 0, transform->GetRight().z);
		rightProj.Normalize();
		forceDir += rightProj;
	}
	float forceMag = 300000;
	rb->SetMaxSpeed(500.f);


	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);

	m_onAirTime += DELTA_TIME;

	if (rb->GetIsFalling()) {
		m_onAirTime = 0.f;
	}

	if (m_onAirTime >= m_onAirCheckTime)
	{
		return make_shared<PlayerJumpLoopState>(m_player);
	}


	return nullptr;

}

void PlayerAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::AIM));
}

shared_ptr<PlayerState> PlayerAimState::OnUpdateState()
{
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && m_player->GetFireReady())
	{
		return make_shared<PlayerFireOnAimState>(m_player);
	}	
	if (!INPUT->GetButton(KEY_TYPE::RBUTTON))
	{
		return make_shared<PlayerAimToIdleState>(m_player);
	}

	return PlayerOnGroundAimState::OnUpdateState();
}

void PlayerIdleToAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::IDLE_TO_AIM));
}

shared_ptr<PlayerState> PlayerIdleToAimState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame())
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
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame())
	{
		return make_shared<PlayerWalkState>(m_player);
	}
	return nullptr;
}

void PlayerFireOnAimState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::FIRE));
	m_player->Fire();
}

shared_ptr<PlayerState> PlayerFireOnAimState::OnUpdateState()
{
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && INPUT->GetButton(KEY_TYPE::RBUTTON) && m_player->GetFireReady())
	{
		m_isFireAnimationAgain = true;
	}
	return PlayerOnGroundAimState::OnUpdateState();
}

shared_ptr<PlayerState> PlayerFireOnAimState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame() || m_isFireAnimationAgain)
	{
		return make_shared<PlayerAimState>(m_player);
	}
	return nullptr;
}
