#include "pch.h"
#include "PlayerOnAirState.h"
#include "PlayerOnGroundAimState.h"
#include "PlayerOnGroundMoveState.h"
#include "Player.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "Input.h"
#include "Manifold.h"

shared_ptr<PlayerState> PlayerOnAirState::OnUpdateState()
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
	rb->SetMaxSpeed(1000.f);


	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);


	return nullptr;
}

void PlayerJumpUpState::OnEnter()
{
	shared_ptr<RigidBody> rb = m_player->GetRigidBody();
	shared_ptr<Transform> transform = m_player->GetTransform();

	Vec3 forceDir = Vec3(0, 1, 0);
	float jumpForceMag = 300000000;

	forceDir.Normalize();
	Vec3 jumpForce = forceDir * jumpForceMag;
	rb->AddForce(jumpForce);

	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::JUMP_UP));
}


shared_ptr<PlayerState> PlayerJumpUpState::OnLateUpdateState()
{
	if (m_player->GetAnimator()->IsAnimationEndOnThisFrame())
	{
		return make_shared<PlayerJumpLoopState>(m_player);
	}
	return nullptr;
}

void PlayerJumpLoopState::OnEnter()
{
	m_player->GetAnimator()->Play(static_cast<uint32>(PLAYER_STATE::JUMP_LOOP));
}

shared_ptr<PlayerState> PlayerJumpLoopState::OnUpdateState()
{
	PlayerOnAirState::OnUpdateState();

	shared_ptr<RigidBody> rb = m_player->GetRigidBody();
	shared_ptr<Transform> transform = m_player->GetTransform();

	for (auto col : *(rb->GetCollideEvent())) {
		Vec3 axis = *col->m_normal;
		if (axis.Dot(Vec3(0, 1, 0)) < -0.5f) {
			return make_shared<PlayerJumpDownState>(m_player);
		}
	}
	return nullptr;
}