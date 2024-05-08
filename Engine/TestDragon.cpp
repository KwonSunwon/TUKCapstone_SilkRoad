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
	//rb->SetStatic(true);

}
void TestDragon::Update()
{


	shared_ptr<RigidBody> rb = GetRigidBody();

	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		rb->SetStatic(false);





}

