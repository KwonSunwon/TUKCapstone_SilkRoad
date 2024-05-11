#include "pch.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "Manifold.h"
#include "Enemy.h"
#include "Timer.h"
#include "Bomb.h"

void PlayerBullet::Update()
{
	ProcessCollides();

	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	Vec3 pos = rb->GetPosition();
	m_lifeTime += DELTA_TIME;

	if (m_lifeTime > m_maxLifeTime)
	{
		rb->MoveTo(Vec3(-1.f, 0, 0));
		rb->SetLinearVelocity(Vec3(0, 0, 0));
		rb->SetStatic(true);
		//GetGameObject()->SetActive(false);
		return;
	}
	if (pos.y < 0 || pos.y >= 50000.f || pos.x < 0 || pos.x >= 50000.f || pos.z < 0 || pos.z >= 50000.f)
	{
		rb->MoveTo(Vec3(-1.f, 0., 0));
		rb->SetLinearVelocity(Vec3(0, 0, 0));
		rb->SetStatic(true);
		//GetGameObject()->SetActive(false);
		return;
	}
	

}

void PlayerBullet::Fire(shared_ptr<Player> shooter, BulletType bulletType)
{
	m_bulletType = bulletType;

	Vec3 cameraLook = shooter->GetPlayerCamera()->GetTransform()->GetLook();
	Vec3 cameraUp = shooter->GetPlayerCamera()->GetTransform()->GetUp();
	cameraLook.Normalize();
	cameraUp.Normalize();

	m_lifeTime = 0.f;

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

void PlayerBullet::ProcessCollides()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptE = col->m_rb2->GetGameObject()->GetMonobehaviour("Enemy");
		shared_ptr<MonoBehaviour> scriptT = col->m_rb2->GetGameObject()->GetMonobehaviour("Terrain");
		switch (m_bulletType)
		{
		case BASIC:
			if (scriptE) {
				shared_ptr<Enemy> enemyScript = dynamic_pointer_cast<Enemy>(scriptE);
				enemyScript->GetDamage(m_attackPower);
				return;
			}
			break;

		case EXPLOSIVE:
			if (scriptE||scriptT) {
				Vec3 pos = (*col->m_contacts)[0];
				m_bomb->SetBombPosition(pos);
				m_bomb->SetBombActive();
				return;
			}

			break;
		default:
			break;
		}
		rb->MoveTo(Vec3(-1.f, 0., 0));
		rb->SetLinearVelocity(Vec3(0, 0, 0));
		rb->SetStatic(true);
		return;
			
			//enemyScript->(0);
		
	}
}

