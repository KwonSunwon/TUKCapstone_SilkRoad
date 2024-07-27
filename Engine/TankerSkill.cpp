#include "pch.h"
#include "TankerSkill.h"
#include "RigidBody.h"
#include "Manifold.h"
#include "GameObject.h"
#include "Enemy.h"
#include "SoundManager.h"
#include "Timer.h"
#include "Transform.h"

void TankerSkill::Awake()
{
	SetMonovihaviourName("TankerSkill");
}

void TankerSkill::Update()
{
	if (GetRigidBody()->GetStatic()) {
		m_isMakeSound = false;
		return;
	}
		
	m_time += DELTA_TIME;
	if (m_time >= 10.f) {
		GetRigidBody()->MoveTo(Vec3(-50000.f, -50000.f, -50000.f));
		GetRigidBody()->SetStatic(true);
		m_time = 0.f;
	}

	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptE = col->m_rb2->GetGameObject()->GetMonobehaviour("Enemy");
		if (scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			if (enemyScript->IsDie()) break;

			Vec3 dir = enemyScript->GetTransform()->GetLocalPosition() - transform->GetLocalPosition();
			dir.Normalize();
			Vec3 power = dir * 100000000;
			enemyScript->GetRigidBody()->AddForce(power);
			enemyScript->GetDamage(10.f);
		}
	
	}

	if (GetRigidBody()->GetIsFalling())
	{
		if (!m_isMakeSound) {
			GET_SINGLE(SoundManager)->soundPlay(Sounds::SKILL_TANKER);
			m_isMakeSound = true;
		}
			
	}


}
