#pragma once
#include "MonoBehaviour.h"

enum BulletType
{
	BASIC,
	EXPLOSIVE,
};

class PlayerBullet : public MonoBehaviour
{
public:
	virtual void Update() override;
	void Fire(class shared_ptr<class Player> shooter, struct FireInfo fireInfo);
	void SetBomb(shared_ptr<class Bomb> bomb) { m_bomb = bomb; }

private:
	void ProcessCollides();

	BulletType m_bulletType = BulletType::BASIC;

	shared_ptr<class Bomb> m_bomb;
	void Fire(class shared_ptr<class Player> shooter);

private:
	float m_speed = 8000.f;
	float m_lifeTime = 0.f;
	float m_maxLifeTime = 3.f;
	float m_attackPower = 20.f;
	float m_bombPower = 100.f;
	float m_bombSize = 500.f;
};