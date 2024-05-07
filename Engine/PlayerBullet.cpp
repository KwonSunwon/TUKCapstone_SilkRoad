#include "pch.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "Manifold.h"
#include "Enemy.h"

void PlayerBullet::Update()
{
	Vec3 pos = GetRigidBody()->GetPosition();
	if (pos.y < 0 || pos.y >= 50000.f || pos.x < 0 || pos.x >= 50000.f || pos.z < 0 || pos.z >= 50000.f)
	{
		GetRigidBody()->MoveTo(Vec3(0, 50000.f, 0));
		GetRigidBody()->SetLinearVelocity(Vec3(0, 0, 0));
		GetRigidBody()->SetStatic(true);
		//GetGameObject()->SetActive(false);
	}
	for (auto col : *(GetRigidBody()->GetCollideEvent())) {
		GetRigidBody()->MoveTo(Vec3(0, 50000.f, 0));
		GetRigidBody()->SetLinearVelocity(Vec3(0, 0, 0));
		GetRigidBody()->SetStatic(true);
		//GetGameObject()->SetActive(false);

	}
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> script =  col->m_rb2->GetGameObject()->GetMonobehaviour("Enemy");
		if (script) {
			shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(script);
			enemyScript->SetMaxWalkSpeed(0);
		}
	}
}

void PlayerBullet::Fire(shared_ptr<Player> shooter)
{
	Vec3 cameraLook = shooter->GetPlayerCamera()->GetTransform()->GetLook();
	Vec3 cameraUp = shooter->GetPlayerCamera()->GetTransform()->GetUp();
	cameraLook.Normalize();
	cameraUp.Normalize();

	GetGameObject()->SetActive(true);

	shared_ptr<RigidBody> rb = GetRigidBody();

	rb->SetStatic(false);
	rb->SetMass(0.003f);
	rb->SetfrictionCoef(0.f);
	rb->MoveTo(shooter->GetPlayerCamera()->GetTransform()->GetWorldPosition() +
		cameraLook * 100.f);
	rb->SetMaxSpeed(8000.f);
	rb->SetMaxAirSpeed(8000.f);


	//GetTransform()->SetLocalPosition(shooter->GetTransform()->GetLocalPosition());
	GetTransform()->SetLocalRotation(shooter->GetTransform()->GetLocalRotation());
	//rb->AddForce(cameraLook * 1000000.f);
	rb->SetLinearVelocity(cameraLook * 8000.f);

}