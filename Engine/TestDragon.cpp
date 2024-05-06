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
	rb->SetStatic(true);

}
void TestDragon::Update()
{


	float dx = 0;
	float dy = 0;
	float dz = 0;
	float speed = 1000;

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();
	

	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement

	rot.y += mouseDelta.x * (double)0.001;

	if (rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;

	GetTransform()->SetLocalRotation(rot);



	if (INPUT->GetButton(KEY_TYPE::KEY_1))
	{
		GetAnimator()->Play(0);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_2))
	{
		GetAnimator()->Play(1);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_3))
	{
		GetAnimator()->Play(2);
	}


	Vec3 forceDir = {0,0,0};

	if (INPUT->GetButton(KEY_TYPE::W)) {
		forceDir+= transform->GetLook();
	}
		
	if (INPUT->GetButton(KEY_TYPE::S)) {
		forceDir -= transform->GetLook();
	}
		

	if (INPUT->GetButton(KEY_TYPE::A)) {
		forceDir -= transform->GetRight();
	}

	if (INPUT->GetButton(KEY_TYPE::D)) {
		forceDir += transform->GetRight();
	}

	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		rb->SetStatic(false);


	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{ }

	
	if (rb->GetXZVelocity().LengthSquared() > pow(speed * 0.6, 2)) {
		if(GetAnimator()->GetCurrentClipIndex()!=2)
			GetAnimator()->Play(2);
	}
	else if (rb->GetLinearVelocity().LengthSquared() > pow(speed * 0.01, 2)) {
		if (GetAnimator()->GetCurrentClipIndex() != 1)
			GetAnimator()->Play(1);
	}
	else {
		if (GetAnimator()->GetCurrentClipIndex() != 0)
			GetAnimator()->Play(0);
	}

	float forceMag = 300000;

	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;
	
	rb->AddForce(force);


}

