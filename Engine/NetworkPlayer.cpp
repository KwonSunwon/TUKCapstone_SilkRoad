#include "pch.h"
#include "NetworkPlayer.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Animator.h"

void NetworkPlayer::Awake()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
}

void NetworkPlayer::Update()
{
}

void NetworkPlayer::LateUpdate()
{
}

void NetworkPlayer::ProcessPacket(shared_ptr<PlayerPacket> packet)
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	rb->SetRotation(packet->m_rotation);
	if (GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
}
