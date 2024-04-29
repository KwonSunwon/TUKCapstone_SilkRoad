#include "pch.h"
#include "Bomb.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Timer.h"
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

	

}

void Bomb::explosion()
{
	if (!isBombActivate)
		return;

	shared_ptr<BaseCollider> bs = GetGameObject()->GetCollider();
	shared_ptr<RigidBody> rb = GetRigidBody();

	rb->MoveTo(Vec3(1500.f, 200.f, 2000.f));
	rb->SetLinearVelocity(Vec3(0.f, 10000.f, 0.f));
	bs->SetRadius(bombTime * 2500);
	bombTime += DELTA_TIME;

	if (bombTime > 2.f) {
		bombTime = 0.f;
		isBombActivate = false;
		bs->SetRadius(0.f);
		rb->MoveTo(Vec3(15000.f, 1500.f, 2000.f));
	}
}
