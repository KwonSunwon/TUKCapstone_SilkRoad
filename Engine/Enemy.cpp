#include "pch.h"
#include "Enemy.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "EnemyState.h"
#include "Timer.h"
#include "Network.h"
#include "Packet.h"
#include "Animator.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TextObject.h"
#include "PathFinding.h"
#include "NetworkObject.h"

void Enemy::Awake()
{
	SetMonovihaviourName("Enemy");

	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
	rb->SetMaxSpeed(m_maxWalkSpeed);
	rb->SetMass(80.f);
	m_pathFinding = make_shared<PathFinding>();
	m_curState = make_shared<EnemyIdleState>(shared_from_this());

}
void Enemy::Update()
{
	if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::GUEST) {
		return;
	}

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if(m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;


	shared_ptr<EnemyState> nextState = m_curState->OnUpdateState();
	if(nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::LateUpdate()
{
	if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::GUEST) {
		return;
	}
	else if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST) {
		if(GET_SINGLE(NetworkManager)->m_isSend) {
			shared_ptr<EnemyPacket> packet = make_shared<EnemyPacket>();
			packet->m_targetId = GetNetworkObject()->GetNetworkId();
			packet->m_position = GetTransform()->GetLocalPosition();
			packet->m_velocity = GetRigidBody()->GetLinearVelocity();
			packet->m_rotation = GetTransform()->GetLocalRotation();
			packet->m_targetPlayerId = GetTargetPlayerIndex();
			packet->m_animationIndex = GetAnimator()->GetCurrentClipIndex();
			//packet->m_hp = m_HP;
			SEND(packet);
		}
	}

	shared_ptr<EnemyState> nextState = m_curState->OnLateUpdateState();
	if(nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Enemy::GetDamage(float damage, bool isPacket)
{
	m_HP -= damage;
}

void Enemy::MakeDamageIndicator(float damage, Vec3 originPos, bool isCri)
{
	shared_ptr<DamageIndicatorTextObject> text = make_shared<DamageIndicatorTextObject>(to_wstring(static_cast<int>(damage)));
	text->SetOriginPosition(originPos);
	if(isCri)
		text->SetBrush("YELLOW");
	else
		text->SetBrush("WHITE");
	GET_SINGLE(SceneManager)->GetActiveScene()->AddTextObject(text);
}

void Enemy::ProcessPacket(shared_ptr<EnemyPacket> packet)
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	transform->SetLocalRotation(packet->m_rotation);
	SetTargetPlayerIndex(packet->m_targetPlayerId);
	if(GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
	//SetHP(packet->m_hp);
}

void Enemy::Fire()
{
	m_fireElapsedTime = 1.f / m_fireRate;
}

std::list<PathNode> Enemy::GetPath()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<RigidBody> rb2 = GetPlayers()[GetTargetPlayerIndex()]->GetRigidBody();
	return m_pathFinding->FindPath(rb->GetPosition(), rb2->GetPosition());
}