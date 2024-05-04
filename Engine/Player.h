#pragma once
#include "MonoBehaviour.h"
#include "PlayerBullet.h"
#include "Camera.h"


class Player : public MonoBehaviour, public std::enable_shared_from_this<Player>
{
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	bool GetFireReady() { return m_fireElapsedTime <= 0.f; }
	void Fire();
	void AddBullet(shared_ptr<PlayerBullet> bullet);
	shared_ptr<Camera> GetPlayerCamera() { return m_playerCamera; }
	void SetPlayerCamera(shared_ptr<Camera> camera) { m_playerCamera = camera; }

private:
	shared_ptr<class PlayerState> m_curState;
	float m_fireRate = 15.f;		// 초당 공격 횟수
	float m_fireElapsedTime = 0.f;

	uint32 m_bulletPivot = 0;
	vector<shared_ptr<PlayerBullet>> m_bullets;
	shared_ptr<Camera> m_playerCamera;
};

