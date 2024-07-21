#include "pch.h"
#include "EnemyHP.h"
#include "Enemy.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"
#include "GameObject.h"
void EnemyHP::Awake()
{
}

void EnemyHP::Update()
{

	float maxHP = m_parentEnemy->GetMaxHP();
	float hp = m_parentEnemy->GetHP();

	if (hp <= 0) {
		hp = 0;
	}

	float percent = hp / maxHP;

	
	GetTransform()->SetLocalScale(Vec3(percent * 100.f, 10.f, 0.f));
	
}

void EnemyHP::LateUpdate()
{
	Vec3 cameraLoc = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->GetWorldPosition();
	Vec3 selfLoc = GetTransform()->GetWorldPosition();
	GetGameObject()-> GetTransform()->LookAt(selfLoc-cameraLoc);
	GetTransform()->SetLocalPosition(m_parentEnemy->GetTransform()->GetLocalPosition() + Vec3(0.f, 250.f, 0.f));
}
