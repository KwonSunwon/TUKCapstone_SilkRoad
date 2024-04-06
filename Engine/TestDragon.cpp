#include "pch.h"
#include "TestDragon.h"
#include "Input.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"

void JumpAnimationEvent()
{

}

void TestDragon::Awake()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetDrag(0.001);
	rb->SetUseGravity(false);
	rb->SetMass(80.0);
	rb->SetIsKinematic(false);
}
void TestDragon::Update()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	if (INPUT->GetButtonDown(KEY_TYPE::W))
	{
		int32 count = GetAnimator()->GetAnimCount();
		//int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		int32 currentIndex = this->GetGameObject()->m_anim;

		int32 index = (currentIndex + 1) % count;
		//GetAnimator()->Play(index);
		
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
	}
	

	if (INPUT->GetButton(KEY_TYPE::W))
		rb->addForce(GetTransform()->GetLook() * 2000 * rb->GetMass(), FORCEMODE::IMPULSE);


	if (INPUT->GetButton(KEY_TYPE::S))
		rb->addForce(-GetTransform()->GetLook() * 2000 * rb->GetMass(), FORCEMODE::IMPULSE);

	if (INPUT->GetButton(KEY_TYPE::A))
		rb->addForce(-GetTransform()->GetRight() * 2000 * rb->GetMass(), FORCEMODE::IMPULSE);


	if (INPUT->GetButton(KEY_TYPE::D))
		rb->addForce(GetTransform()->GetRight() * 2000 * rb->GetMass(), FORCEMODE::IMPULSE);

	static float a = 0;


	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		rb->SetUseGravity(true);
		// a += 0.01;

	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
		if (rb->GetIsLanding()) {
			GetAnimator()->Play(1);
			GetAnimator()->SetEventFunction(1, 0.5f, [this,rb](){rb->addForce(GetTransform()->GetUp() * 700, FORCEMODE::VELOCITYCHANGE); });
		}
			


	GetTransform()->SetLocalRotation(Vec3(0, a, 0));

	//if (INPUT->GetButton(KEY_TYPE::A))
	//	pos -= GetTransform()->GetRight() * m_speed * DELTA_TIME;

	//if (INPUT->GetButton(KEY_TYPE::D))
	//	pos += GetTransform()->GetRight() * m_speed * DELTA_TIME;

	

}

