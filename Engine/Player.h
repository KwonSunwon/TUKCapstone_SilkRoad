#pragma once
#include "MonoBehaviour.h"


class Player : public MonoBehaviour, public std::enable_shared_from_this<Player>
{
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	bool GetFireReady() { return m_fireElapsedTime <= 0.f; }
	void Fire() { m_fireElapsedTime = 1.f / m_fireRate; }

private:
	shared_ptr<class PlayerState> m_curState;
	float m_fireRate = 1.f;		// 초당 공격 횟수
	float m_fireElapsedTime = 0.f;
};

