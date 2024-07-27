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
#include "InteractiveObject.h"
#include "UpgradeManager.h"
#include "Resources.h"
#include "Bomb.h"
#include "NetworkObject.h"

void Player::Awake()
{
	GET_SINGLE(UpgradeManager)->SetClass();
	GET_SINGLE(UpgradeManager)->SetStat();

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
	SkillManage();
	ProcessGetItem();
	InteracitveObjectPick();
	if(INPUT->GetButtonDown(KEY_TYPE::Q))
	{
		Skill();

		float minDistance = FLT_MAX;
		vector<shared_ptr<GameObject>>gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveGameObjects();
		Vec3 cameraPos = m_playerCamera->GetTransform()->GetWorldPosition();
		Vec3 cameraDir = m_playerCamera->GetTransform()->GetLook();
		shared_ptr<GameObject> picked;

		Vec4 rayOrigin = Vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
		Vec4 rayDir = Vec4(cameraDir.x, cameraDir.y, cameraDir.z, 0.0f);
		for(auto& gameObject : gameObjects) {


			float distance = 0.f;
			if(gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
				continue;

			if(gameObject->GetMonobehaviour("Player"))
				continue;

			if(distance > 250.f) {
				continue;
			}

			if(distance < minDistance)
			{
				minDistance = distance;
				picked = gameObject;
			}
		}

		if(picked)
			picked->GetInteractiveObject()->InteractiveFunction();


	}


	if(INPUT->GetButtonDown(KEY_TYPE::E))
	{
		int chaClass = GET_SINGLE(UpgradeManager)->GetClass() + 1;
		if(chaClass == 9) chaClass = 5;

		GET_SINGLE(UpgradeManager)->ClassChange(chaClass);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::Z))
	{

		GET_SINGLE(UpgradeManager)->AddGold(100.f);
	}

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();

	if(m_fireElapsedTime > 0)
		m_fireElapsedTime -= DELTA_TIME;

	if(m_skillRemainingTime > 0)
		m_skillRemainingTime -= DELTA_TIME;

	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement

	rot.y += mouseDelta.x * (double)0.001;
	if(rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;


	GetTransform()->SetLocalRotation(rot);


	if(GET_SINGLE(Input)->GetButtonDown(KEY_TYPE::KEY_3))
		m_hp -= 30.f;


	shared_ptr<PlayerState> nextState = m_curState->OnUpdateState();
	if(nextState)
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
	if(nextState)
	{
		m_curState->OnExit();
		m_curState = nextState;
		m_curState->OnEnter();
	}

	if(GET_SINGLE(NetworkManager)->m_isSend) {
		shared_ptr<PlayerPacket> playerPacket = make_shared<PlayerPacket>();
		playerPacket->m_targetId = GET_SINGLE(NetworkManager)->m_networkId;
		playerPacket->m_position = rb->GetPosition();
		playerPacket->m_velocity = rb->GetLinearVelocity();
		playerPacket->m_rotation = transform->GetLocalRotation();
		playerPacket->m_animationIndex = GetAnimator()->GetCurrentClipIndex();
		playerPacket->m_hp = m_hp;
		playerPacket->m_maxHp = m_maxHP;
		SEND(playerPacket)
	}
}

void Player::Fire()
{
	//BulletType bulletType = CalcBulletType();
	//m_fireInfo.bulletType = bulletType;
	m_fireElapsedTime = 1.f / m_fireRate;
	GET_SINGLE(SoundManager)->soundPlay(Sounds::WEAPON_ASSULT_FIRE);

	float minDistance = FLT_MAX;
	vector<shared_ptr<GameObject>>gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetCollidableGameObjects();
	Vec3 cameraPos = m_playerCamera->GetTransform()->GetWorldPosition();
	Vec3 cameraDir = m_playerCamera->GetTransform()->GetLook();
	shared_ptr<GameObject> picked;
	Vec4 rayOrigin = Vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	Vec4 rayDir = Vec4(cameraDir.x, cameraDir.y, cameraDir.z, 0.0f);

	for(auto& gameObject : gameObjects) {

		float distance = 0.f;
		if(gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if(gameObject->GetMonobehaviour("Player"))
			continue;

		if(gameObject->GetMonobehaviour("Bomb"))
			continue;

		if(distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	if(!picked)
		return;

	shared_ptr<MonoBehaviour> scriptE = picked->GetMonobehaviour("Enemy");
	Vec3 damagePos = cameraPos + cameraDir * minDistance;
	float finalDamage = 0.f;


	switch(m_fireInfo.bulletType) {
	case BulletType::BASIC:
		if(scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			if(enemyScript->IsDie()) return;

			finalDamage = CalcDamage();

			enemyScript->GetDamage(finalDamage);
			enemyScript->MakeDamageIndicator(finalDamage, damagePos, m_isCritical);
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_HIT_ENEMY);
		}
		break;

	case BulletType::EXPLOSIVE:
		if(scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			if(enemyScript->IsDie()) return;


			m_fireInfo.bulletType = BASIC;

			m_bomb->SetBombSize(500.f);
			m_bomb->SetBombPosition(damagePos);
			m_bomb->SetBombActive();

			//enemyScript->GetDamage(m_fireInfo.explosionDamage);
			//enemyScript->MakeDamageIndicator(m_fireInfo.explosionDamage, damagePos,m_isCritical);

			if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
				shared_ptr<SkillPacket> packet = make_shared<SkillPacket>();
				packet->m_skillType = GET_SINGLE(UpgradeManager)->GetClass();
				packet->m_pos = damagePos;
				packet->m_isBomb = true;
				SEND(packet);
			}
		}
		break;

	}

	if(!picked->GetRigidBody()->GetStatic()) {
		picked->GetRigidBody()->AddForce(Vec3(rayDir) * m_knockBackPower * 1000000.f);
	}

	if(scriptE == nullptr)
		return;
	shared_ptr<EnemyHitPacket> packet = make_shared<EnemyHitPacket>();
	packet->m_targetId = picked->GetNetworkObject()->GetNetworkId();
	packet->m_damage = finalDamage;
	packet->m_rayDir = Vec3(rayDir);
	packet->m_knockBackPower = m_knockBackPower;
	SEND(packet);

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


void Player::SetSkillObject(int id, shared_ptr<GameObject> gm)
{
	switch(id)
	{
	case 0:
		m_guardObject = gm;
		break;

	case 1:
		m_healObject = gm;
		break;

	case 2:
		m_bombObject = gm;
		break;

	case 3:
		m_dealObject = gm;
		break;




	default:
		break;
	}
}

void Player::InteracitveObjectPick()
{
	float minDistance = FLT_MAX;
	vector<shared_ptr<GameObject>>gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveGameObjects();
	Vec3 cameraPos = m_playerCamera->GetTransform()->GetWorldPosition();
	Vec3 cameraDir = m_playerCamera->GetTransform()->GetLook();
	shared_ptr<GameObject> picked;

	Vec4 rayOrigin = Vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	Vec4 rayDir = Vec4(cameraDir.x, cameraDir.y, cameraDir.z, 0.0f);
	for(auto& gameObject : gameObjects) {


		float distance = 0.f;
		if(gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if(gameObject->GetMonobehaviour("Player"))
			continue;

		if(distance > 250.f) {
			continue;
		}

		if(distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	if(!picked)
	{
		GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetVisible(false);
		return;
	}

	picked->GetInteractiveObject()->PrintInteractiveText();

	/*shared_ptr<MonoBehaviour> scriptI = picked->GetMonobehaviour("InteractiveObject");
	shared_ptr<InteractiveObject> interactiveObjectScript = dynamic_pointer_cast<InteractiveObject>(scriptI);
	interactiveObjectScript->PrintInteractiveText();*/


}

void Player::ProcessGetItem()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	for(auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptI = col->m_rb2->GetGameObject()->GetMonobehaviour("Item");
		if(scriptI) {
			shared_ptr<Item> itemScript = dynamic_pointer_cast<Item>(scriptI);

			itemScript->GetRigidBody()->SetStatic(true);
			itemScript->AddGetItemText();
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);

			col->m_rb2->MoveTo(Vec3(-50000, 1000000, 0));
			GET_SINGLE(UpgradeManager)->ApplyItem(itemScript->GetItemID());
		}
	}
}


float Player::CalcDamage()
{

	bool isCriticalHit = (static_cast<float>(rand()) / RAND_MAX) < m_criticalPercentage;


	int minDamage = m_bulletDamage * (1.f - m_minusDamage);
	int maxDamage = m_bulletDamage * (1.f + m_plusDamage);
	int baseDamage = minDamage + rand() % (maxDamage - minDamage + 1);


	if(isCriticalHit) {
		m_isCritical = true;
		return baseDamage * m_criticalDamage;
	}
	else {
		m_isCritical = false;
		return baseDamage;
	}
}

BulletType Player::CalcBulletType()
{
	/*if (m_itemLevels[0] == 0)
		return BulletType::BASIC;

	if (m_fireTime % (5 - m_itemLevels[0]) == 0)
		return BulletType::EXPLOSIVE;*/

	return BulletType::BASIC;
}

void Player::Skill()
{
	if(GET_SINGLE(SceneManager)->GetActiveScene()->GetSceneName() == "Lobby")
		return;

	if(m_skillRemainingTime > 0)
		return;

	m_skillRemainingTime = m_skillCoolTime;

	switch(GET_SINGLE(UpgradeManager)->GetClass()) {
	case CharacterClass::DEALER:
		SkillDealer();
		break;

	case CharacterClass::HEALER:
		SkillHealer();
		break;

	case CharacterClass::LAUNCHER:
		SkillLauncher();
		break;

	case CharacterClass::TANKER:
		SkillTanker();
		break;

	default:
		break;
	}

	if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
		shared_ptr<SkillPacket> packet = make_shared<SkillPacket>();
		packet->m_skillType = GET_SINGLE(UpgradeManager)->GetClass();
		auto transform = GetTransform();
		packet->m_pos = transform->GetLocalPosition();
		packet->m_look = transform->GetLook();
		packet->m_dropPos = packet->m_pos + packet->m_look * 700.f + Vec3(0.f, 1000.f, 0.f);
		packet->m_isBomb = false;
		SEND(packet);
	}
}

void Player::SkillDealer()
{
	GET_SINGLE(Resources)->Get<Material>(L"Final")->SetInt(0, 1);
	m_isRage = true;
}


void Player::SkillHealer()
{
	if(!m_healObject)
		return;

	shared_ptr<Transform> transform = GetTransform();
	Vec3 pos = transform->GetLocalPosition();
	Vec3 look = transform->GetLook();
	Vec3 dropPos = pos + look * 700.f + Vec3(0.f, 1000.f, 0.f);

	//m_guardObject->GetRigidBody()->SetStatic(false);
	m_healObject->GetRigidBody()->MoveTo(pos);
	//m_guardObject->GetTransform()->LookAt(look);
}

void Player::SkillLauncher()
{
	m_fireInfo.bulletType = EXPLOSIVE;
}



void Player::SkillTanker()
{
	if(!m_guardObject)
		return;

	shared_ptr<Transform> transform = GetTransform();
	Vec3 pos = transform->GetLocalPosition();
	Vec3 look = transform->GetLook();
	Vec3 dropPos = pos + look * 700.f + Vec3(0.f, 1000.f, 0.f);

	m_guardObject->GetRigidBody()->SetStatic(false);
	m_guardObject->GetRigidBody()->MoveTo(dropPos);
	m_guardObject->GetTransform()->LookAt(look);
}

void Player::SkillManage()
{


	if(m_isRage) {
		GET_SINGLE(UpgradeManager)->SetStat();
		m_bulletDamage *= 1.5f;
		m_fireRate *= 1.5f;
		m_criticalPercentage = 1.f;

		m_rageTime += DELTA_TIME;
		float dist = sin(m_rageTime * 3.14f / 5.f) * 3.f;
		GET_SINGLE(Resources)->Get<Material>(L"Final")->SetFloat(0, dist);

	}
	if(m_rageTime > 5.f) {
		GET_SINGLE(UpgradeManager)->SetStat();
		m_isRage = false;
		m_rageTime = 0.f;
		GET_SINGLE(Resources)->Get<Material>(L"Final")->SetInt(0, 0);
	}
}

void Player::NetworkSkill(shared_ptr<SkillPacket> packet)
{
	switch(packet->m_skillType) {
	case CharacterClass::HEALER:
		if(!m_healObject)
			return;
		m_healObject->GetRigidBody()->MoveTo(packet->m_pos);
		break;
	case CharacterClass::TANKER:
		if(!m_guardObject)
			return;
		m_guardObject->GetRigidBody()->SetStatic(false);
		m_guardObject->GetRigidBody()->MoveTo(packet->m_dropPos);
		m_guardObject->GetTransform()->LookAt(packet->m_look);
		break;
	case CharacterClass::LAUNCHER:
		if(packet->m_isBomb) {
			m_bomb->SetBombSize(500.f);
			m_bomb->SetBombPosition(packet->m_pos);
			m_bomb->SetBombActive();
		}
		break;
	default:
		break;
	}
}