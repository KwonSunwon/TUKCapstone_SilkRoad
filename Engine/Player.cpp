#include "pch.h"
#include "Player.h"
#include "Input.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
void Player::Awake()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(true);

}
void Player::Update()
{

	shared_ptr<Transform> transform = GetTransform();
	shared_ptr<RigidBody> rb = GetRigidBody();


	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement

	rot.y += mouseDelta.x * (double)0.001;

	if (rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;

	GetTransform()->SetLocalRotation(rot);




	Vec3 forceDir = { 0,0,0 };

	if (INPUT->GetButton(KEY_TYPE::W)) {
		forceDir += transform->GetLook();
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
	{
	}


	

	float forceMag = 300000;

	forceDir.Normalize();
	Vec3 force = forceDir * forceMag;

	rb->AddForce(force);


}
