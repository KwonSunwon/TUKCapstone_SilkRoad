#include "pch.h"
#include "Bomb.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Timer.h"
#include "Manifold.h"
#include "Enemy.h"
void Bomb::Awake()
{
	SetMonovihaviourName("Bomb");
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(true);

}
void Bomb::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		m_isBombActivate = true;
	}

	explosion();


	for (auto col : *(GetRigidBody()->GetCollideEvent())) {
		
		col->m_rb2->SetfrictionCoef(0.f);
		col->m_rb2->SetMaxSpeed(m_bombPower);
		

		Vec3 normal = *col->m_normal;


		Vec3 relativeVelocity = col->m_rb2->GetLinearVelocity() - normal * 2000;
		if (relativeVelocity.Dot(normal) > 0.f) {
			continue;
		}

		float e = min(col->m_rb1->GetRestitution(), col->m_rb2->GetRestitution());
		float j = -(1.f + e) * relativeVelocity.Dot(normal);
		j /= col->m_rb1->GetInvMass() + col->m_rb2->GetInvMass();
		Vec3 impulse = (normal)*j;

		col->m_rb2->SetLinearVelocity(col->m_rb2->GetLinearVelocity() + impulse * col->m_rb2->GetInvMass());


		shared_ptr<MonoBehaviour> scriptE = col->m_rb2->GetGameObject()->GetMonobehaviour("Enemy");
		if (scriptE) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
			enemyScript->GetDamage(100.f*DELTA_TIME);
			enemyScript->MakeDamageIndicator(100.f*DELTA_TIME, col->m_rb2->GetPosition() + Vec3(0.f, 150.f, 0.f), false);
		}
	}

}

void Bomb::explosion()
{
	if (!m_isBombActivate)
		return;

	shared_ptr<BaseCollider> bs = GetGameObject()->GetCollider();
	shared_ptr<RigidBody> rb = GetRigidBody();

	rb->MoveTo(m_bombPos);
	rb->SetLinearVelocity(Vec3(0.f, 10000.f, 0.f));


	m_bombTime += DELTA_TIME * 3.14 / m_bombMaxTime;
	float size =  sin(m_bombTime) * m_bombSize;
	


	GetTransform()->SetLocalScale(Vec3(size, size, size));
	bs->SetRadius(size/2);
	

	if (m_bombTime > 3.14f) {
		m_bombTime = 0.f;
		m_isBombActivate = false;
		//bs->SetRadius(0.f);
		//rb->MoveTo(Vec3(10000, 300, 5000));
		rb->MoveTo(Vec3(22500 + 500,- 1200.f, 15000 + 200));
	}
}
