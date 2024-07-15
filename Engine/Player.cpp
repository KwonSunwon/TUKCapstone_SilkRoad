#include "pch.h"
#include "Player.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "PlayerOnGroundMoveState.h"
#include "PlayerOnGroundAImState.h"
#include "PlayerOnAirState.h"
#include "PlayerBullet.h"
#include "Network.h"
#include "Packet.h"
#include "Animator.h"
#include "Manifold.h"
#include "Item.h"
#include "TextObject.h"
#include "UIObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Enemy.h"
#include "BaseCollider.h"
#include "SoundManager.h"

void Player::Awake()
{
	SetMonovihaviourName("Player");
	shared_ptr<RigidBody> rb = GetRigidBody();
	//rb->SetStatic(true);
	m_curState = make_shared<PlayerWalkState>(shared_from_this());
	m_fireInfo.bulletDamage = 20.f;
	m_fireInfo.bulletType = BulletType::BASIC;
	m_fireInfo.explosionDamage = 100.f;
	m_fireInfo.explosionSize = 500.f;
}
void Player::Update()
{
	ProcessGetItem();

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if (m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;

	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement

	rot.y += mouseDelta.x * (double)0.001;
	if (rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;


	GetTransform()->SetLocalRotation(rot);


	shared_ptr<PlayerState> nextState = m_curState->OnUpdateState();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}
}

void Player::LateUpdate()
{
	shared_ptr<PlayerState> nextState = m_curState->OnLateUpdateState();
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	if (nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}

	shared_ptr<PlayerPacket> playerPacket = make_shared<PlayerPacket>();
	playerPacket->m_targetId = GET_SINGLE(NetworkManager)->m_networkId;
	playerPacket->m_position = rb->GetPosition();
	playerPacket->m_velocity = rb->GetLinearVelocity();
	playerPacket->m_rotation = transform->GetLocalRotation();
	playerPacket->m_animationIndex = GetAnimator()->GetCurrentClipIndex();
	SEND(playerPacket)
}

void Player::Fire()
{
	BulletType bulletType = CalcBulletType();
	m_fireInfo.bulletType = bulletType;
	m_fireElapsedTime = 1.f / m_fireRate;
	GET_SINGLE(SoundManager)->soundPlay(Sounds::WEAPON_ASSULT_FIRE);

	float minDistance = FLT_MAX;
	vector<shared_ptr<GameObject>>gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetCollidableGameObjects();
	Vec3 cameraPos = m_playerCamera->GetTransform()->GetWorldPosition();
	Vec3 cameraDir = m_playerCamera->GetTransform()->GetLook();
	shared_ptr<GameObject> picked;
	for (auto& gameObject : gameObjects) {
		Vec4 rayOrigin = Vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
		Vec4 rayDir = Vec4(cameraDir.x, cameraDir.y, cameraDir.z, 0.0f);

		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (gameObject->GetMonobehaviour("Player"))
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}
	
	if (!picked)
		return;

	shared_ptr<MonoBehaviour> scriptE = picked->GetMonobehaviour("Enemy");
	Vec3 damagePos = cameraPos + cameraDir * minDistance;

	switch (m_fireInfo.bulletType) {
	case BulletType::BASIC:
		if (scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			if (enemyScript->IsDie()) return;
			enemyScript->GetDamage(m_fireInfo.bulletDamage);
			enemyScript->MakeDamageIndicator(m_fireInfo.bulletDamage, damagePos);
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_HIT_ENEMY);
		}
		break;

	case BulletType::EXPLOSIVE:
		if (scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			if (enemyScript->IsDie()) return;
			enemyScript->GetDamage(m_fireInfo.explosionDamage);
			enemyScript->MakeDamageIndicator(m_fireInfo.explosionDamage, damagePos);
		}
		break;

	}


	
	//총알 사용할때 코드
	//m_bullets[m_bulletPivot++]->Fire(shared_from_this(), m_fireInfo);
	//m_fireTime++;


	/*if (m_bulletPivot >= m_bullets.size())
		m_bulletPivot = 0;*/
}

void Player::AddBullet(shared_ptr<class PlayerBullet> bullet)
{
	m_bullets.push_back(bullet);
}


void Player::ProcessGetItem()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptI = col->m_rb2->GetGameObject()->GetMonobehaviour("Item");
		if (scriptI) {
			shared_ptr<Item> itemScript = dynamic_pointer_cast<Item>(scriptI);
			m_itemLevels[itemScript->GetItemID()]++;

			itemScript->AddGetItemText();
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);

			col->m_rb2->MoveTo(Vec3(0, 1000000, 0));
			CalcBulletStat(itemScript->GetItemID());
		}
	}
}



void Player::CalcBulletStat(int id)
{
	switch (id) {
	case 0:
		break;
	case 1:
		m_fireRate += 3;
		break;

	case 2:
		m_fireInfo.bulletDamage += 4;
		break;

	case 3:
		m_fireInfo.explosionDamage += 20;
		break;

	case 4:
		m_fireInfo.explosionSize += 200;
		break;



	}
}

BulletType Player::CalcBulletType()
{
	if (m_itemLevels[0] == 0)
		return BulletType::BASIC;

	if (m_fireTime % (5 - m_itemLevels[0]) == 0)
		return BulletType::EXPLOSIVE;

	return BulletType::BASIC;
}
