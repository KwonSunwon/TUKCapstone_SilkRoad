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
#include "Manifold.h"

shared_ptr<PlayerState> PlayerOnGroundMoveState::OnUpdateState()
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
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		rb->SetStatic(false);
	}

	float forceMag = 300000;

	rb->SetMaxSpeed(1300.f);

	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);

	m_onAirTime += DELTA_TIME;

	for (auto col : *(rb->GetCollideEvent())) {
		Vec3 axis = *col->m_normal;
		if (axis.Dot(Vec3(0,1,0)) < -0.5f) {
			m_onAirTime = 0.f;
		}
	}

	if (m_onAirTime >= m_onAirCheckTime)
	{
		return make_shared<PlayerJumpLoopState>(m_player);
	}

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
	if (INPUT->GetButton(KEY_TYPE::SPACE))
	{
		return make_shared<PlayerJumpUpState>(m_player);
	}
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
	return PlayerOnGroundMoveState::OnUpdateState();;
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
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && m_player->GetFireReady())
	{
		m_isFireAnimationAgain = true;
	}
	return PlayerOnGroundMoveState::OnUpdateState();
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
	shared_ptr<PlayerState> res = PlayerOnGroundMoveState::OnUpdateState();
	if (res)
	{
		return res;
	}

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

void PlayerJumpDownState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::JUMP_DOWN));
}

shared_ptr<PlayerState> PlayerJumpDownState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame())
	{
		return make_shared<PlayerWalkState>(m_player);
	}
	return nullptr;
}
