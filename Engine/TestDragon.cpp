#include "pch.h"
#include "TestDragon.h"
#include "Input.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
void JumpAnimationEvent()
{

}

void TestDragon::Awake()
{
	shared_ptr<RigidBody> rb = GetRigidBody();

}
void TestDragon::Update()
{

	float dx = 0;
	float dy = 0;
	float dz = 0;
	float speed = 1000;

	shared_ptr<RigidBody> rb = GetRigidBody();
	if (INPUT->GetButtonDown(KEY_TYPE::W))
	{
		//int32 count = GetAnimator()->GetAnimCount();
		//int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		//int32 currentIndex = this->GetGameObject()->m_anim;

		//int32 index = (currentIndex + 1) % count;
		//GetAnimator()->Play(index);
		
		//etAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
	}
	

	if (INPUT->GetButton(KEY_TYPE::W))
		dy++;


	if (INPUT->GetButton(KEY_TYPE::S))
		dy--;

	if (INPUT->GetButton(KEY_TYPE::A))
		dx--;


	if (INPUT->GetButton(KEY_TYPE::D))
		dx++;

	if (INPUT->GetButton(KEY_TYPE::Q))
		dz++;

	if (INPUT->GetButton(KEY_TYPE::E))
		dz--;


	static float a = 0;
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		 a += 0.0001;

	GetTransform()->SetLocalRotation(Vec3(a, 0, 0));

	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{ }

	Vec3 dir{ dx,dz,dy };
	dir.Normalize();

	Vec3 velocity = dir * speed * DELTA_TIME;

	rb->Move(velocity);


}

