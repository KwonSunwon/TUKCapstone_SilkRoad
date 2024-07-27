#include "pch.h"
#include "HealerSkill.h"
#include "RigidBody.h"
#include "Manifold.h"
#include "GameObject.h"
#include "Enemy.h"
#include "SoundManager.h"
#include "Timer.h"
#include "Transform.h"
#include "Player.h"

void HealerSkill::Awake()
{
	SetMonovihaviourName("HealerSkill");
}

void HealerSkill::Update()
{
	m_time += DELTA_TIME;
	if (m_time >= 10.f) {
		GetRigidBody()->MoveTo(Vec3(-50000.f, -50000.f, -50000.f));
		GetRigidBody()->SetStatic(true);
		m_time = 0.f;
	}

	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	for (auto col : *(rb->GetCollideEvent())) {
		shared_ptr<MonoBehaviour> scriptP = col->m_rb2->GetGameObject()->GetMonobehaviour("Player");
		if (scriptP) {
			shared_ptr<Player> playerScript = dynamic_pointer_cast<Player>(scriptP);
			playerScript->SetHP(playerScript->GetHP() + DELTA_TIME * 10.f);

		}

	}

}
