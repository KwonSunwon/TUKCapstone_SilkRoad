#include "pch.h"
#include "NetworkPlayer.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Animator.h"
#include "GameObject.h"
#include "Network.h"

#include <chrono>

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
	/*auto timeStamp = chrono::system_clock::now();
	string debugText = " Player " + to_string(packet->m_targetId) + " position: " + to_string(packet->m_position.x) + ", " + to_string(packet->m_position.y) + ", " + to_string(packet->m_position.z);
	debugText += " velocity: " + to_string(packet->m_velocity.x) + ", " + to_string(packet->m_velocity.y) + ", " + to_string(packet->m_velocity.z) + '\n';
	GET_SINGLE(NetworkManager)->m_log << timeStamp << debugText;*/

	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	auto rotation = Vec3(0, packet->m_rotation.y, 0);
	transform->SetLocalRotation(rotation);
	//rb->SetRotation(packet->m_rotation);
	if(GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
}
