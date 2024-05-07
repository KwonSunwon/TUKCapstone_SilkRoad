#pragma once
#include "MonoBehaviour.h"

class PlayerBullet : public MonoBehaviour
{
public:
	virtual void Update() override;
	void Fire(class shared_ptr<class Player> shooter);

private:
	float m_speed = 8000.f;
	float m_lifeTime = 0.f;
	float m_maxLifeTime = 3.f;
	float m_attackPower = 20.f;
};