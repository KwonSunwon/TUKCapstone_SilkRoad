#include "pch.h"
#include "TestDragon.h"
#include "Input.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
void TestDragon::Update()
{
	/*if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
	}*/
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->m_drag = 0.001;
	rb->m_useGravity = true;
	rb->m_mass = 80.0;
	rb->m_isKinematic = false;

	if (INPUT->GetButton(KEY_TYPE::W))
		rb->addForce(GetTransform()->GetLook() * 20, FORCEMODE::VELOCITYCHANGE);


	if (INPUT->GetButton(KEY_TYPE::S))
		rb->addForce(GetTransform()->GetLook() * -20, FORCEMODE::VELOCITYCHANGE);

	if (INPUT->GetButton(KEY_TYPE::A))
		rb->addForce(GetTransform()->GetRight() * 20, FORCEMODE::VELOCITYCHANGE);


	if (INPUT->GetButton(KEY_TYPE::D))
		rb->addForce(GetTransform()->GetRight() * -20, FORCEMODE::VELOCITYCHANGE);

	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		rb->addForce(GetTransform()->GetUp() * 200000, FORCEMODE::IMPULSE);

	//if (INPUT->GetButton(KEY_TYPE::A))
	//	pos -= GetTransform()->GetRight() * m_speed * DELTA_TIME;

	//if (INPUT->GetButton(KEY_TYPE::D))
	//	pos += GetTransform()->GetRight() * m_speed * DELTA_TIME;



}

