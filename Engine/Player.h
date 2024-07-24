#pragma once
#include "MonoBehaviour.h"
#include "PlayerBullet.h"
#include "Camera.h"
static array<int, 17> m_itemLevels;
static array<int, 10> m_upgradeLevels;

struct FireInfo {
	BulletType bulletType = BulletType::BASIC;
	float bulletDamage = 0.f;
	
	float explosionDamage = 0.f;
	float explosionSize = 0.f;
};


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

public:
	float GetHP() { return m_hp; }
	float GetMaxHP() { return m_maxHP; }
	float GetDefense() { return m_defense; }
	float GetHPRegen() { return m_hpRegen; }
	float GetKnockBackPower() { return m_knockBackPower; }
	float GetCriticalPercentage() { return m_criticalPercentage; }
	float GetCriticalDamage() { return m_criticalDamage; }
	float GetMinusDamage() { return m_minusDamage; }
	float GetPlusDamage() { return m_plusDamage; }

	float GetMaxWalkSpeed() { return m_maxWalkSpeed; }
	float GetMaxJumpSpeed() { return m_maxJumpSpeed; }
	float GetMaxAimSpeed() { return m_maxAimSpeed; }


	
private:
	shared_ptr<class PlayerState> m_curState;
	shared_ptr<Camera> m_playerCamera;

private:
	FireInfo m_fireInfo;
	float m_fireRate = 3.f;		// 초당 공격 횟수
	float m_fireElapsedTime = 0.f;
	int m_fireTime = 0;

	uint32 m_bulletPivot = 0;
	vector<shared_ptr<PlayerBullet>> m_bullets;

	
private:
	//Player Stat
	float m_hp					= 100.f;
	float m_maxHP				= 100.f;				
	float m_defense				= 5.f;
	float m_hpRegen				= 5.f;

	float m_maxWalkSpeed		= 1300.f;
	float m_maxJumpSpeed		= 1000.f;
	float m_maxAimSpeed			= 500.f;

	float m_minusDamage			= 0.2f;
	float m_plusDamage			= 0.2f;

	float m_criticalPercentage	= 0.3f;
	float m_criticalDamage		= 1.7f;

	float m_knockBackPower		= 10.f;

	bool m_isCritical = false;

	
	

private:
	void InteracitveObjectPick();
	void ProcessGetItem();
	void ApplyItem(int id);
	float CalcDamage();
	BulletType CalcBulletType();
};

