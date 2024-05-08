#include "pch.h"
#include "Bomb.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Timer.h"
#include "Manifold.h"
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
		//col->m_rb2->SetLinearVelocity(*col->m_normal * m_bombPower);
		col->m_rb2->SetMaxSpeed(m_bombPower);
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
		rb->MoveTo(Vec3(22500 + 500, 1200.f, 15000 + 200));
	}
}
