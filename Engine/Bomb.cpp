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
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(true);

}
void Bomb::Update()
{



	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		isBombActivate = true;
	}
	explosion();


	for (auto col : *(GetRigidBody()->GetCollideEvent())) {
		col->m_rb2->SetfrictionCoef(0.f);
		col->m_rb2->SetLinearVelocity(*col->m_normal * 10000.f);
		col->m_rb2->SetMaxSpeed(3000.f);
	}

}

void Bomb::explosion()
{
	if (!isBombActivate)
		return;

	shared_ptr<BaseCollider> bs = GetGameObject()->GetCollider();
	shared_ptr<RigidBody> rb = GetRigidBody();

	rb->MoveTo(Vec3(3000.f, -100.f, 5000.f));
	rb->SetLinearVelocity(Vec3(0.f, 10000.f, 0.f));


	bombTime +=  DELTA_TIME * 3.14;
	float size =  sin(bombTime) * 2500;
	


	GetTransform()->SetLocalScale(Vec3(size, size, size));
	bs->SetRadius(size/2);
	//bombTime += DELTA_TIME;

	if (bombTime > 3.14f) {
		bombTime = 0.f;
		isBombActivate = false;
		//bs->SetRadius(0.f);
		rb->MoveTo(Vec3(10000, 300, 5000));
	}
}
