#pragma once
#include "MonoBehaviour.h"
#include "PlayerBullet.h"
#include "Camera.h"

struct FireInfo {
	BulletType bulletType = BulletType::BASIC;
	float bulletDamage = 0.f;

	float explosionDamage = 0.f;
	float explosionSize = 0.f;
};

class Bomb;

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

	void ApplyDamage(float damage);

public:
	float GetHP() { return m_hp; }
	void SetHP(float hp) { m_hp = hp; if(m_hp > m_maxHP) m_hp = m_maxHP; }

	float GetMaxHP() { return m_maxHP; }
	void SetMaxHP(float maxHP) { m_maxHP = maxHP; }

	float GetDefense() { return m_defense; }
	void SetDefense(float defense) { m_defense = defense; }

	float GetHpRegen() { return m_hpRegen; }
	void SetHpRegen(float hpRegen) { m_hpRegen = hpRegen; }

	float GetMaxWalkSpeed() { return m_maxWalkSpeed; }
	void SetMaxWalkSpeed(float maxWalkSpeed) { m_maxWalkSpeed = maxWalkSpeed; }

	float GetMaxJumpSpeed() { return m_maxJumpSpeed; }
	void SetMaxJumpSpeed(float maxJumpSpeed) { m_maxJumpSpeed = maxJumpSpeed; }

	float GetMaxAimSpeed() { return m_maxAimSpeed; }
	void SetMaxAimSpeed(float maxAimSpeed) { m_maxAimSpeed = maxAimSpeed; }

	float GetCriPercent() { return m_criticalPercentage; }
	void SetCriPercent(float criticalPercentage) { m_criticalPercentage = criticalPercentage; }

	float GetCriDamage() { return m_criticalDamage; }
	void SetCriDamage(float criDamage) { m_criticalDamage = criDamage; }

	float GetFireRate() { return m_fireRate; }
	void SetFireRate(float fireRate) { m_fireRate = fireRate; }

	float GetBulletDamage() { return m_bulletDamage; }
	void SetBulletDamage(float bulletDamage) { m_bulletDamage = bulletDamage; }

	float GetMinusDamage() { return m_minusDamage; }
	void SetMinusDamage(float minusDamage) { m_minusDamage = minusDamage; }

	float GetPlusDamage() { return m_plusDamage; }
	void SetPlusDamage(float plusDamage) { m_plusDamage = plusDamage; }

	float GetSkillCoolTime() { return m_skillCoolTime; }
	float GetSkillRemainingTime() { return m_skillRemainingTime; }

	void SetSkillObject(int id, shared_ptr<GameObject> gm);
	void SetBomb(shared_ptr<Bomb> bomb) { m_bomb = bomb; }
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

	float m_skillCoolTime = 12.f;
	float m_skillRemainingTime = 0.f;


	
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
	float m_bulletDamage		= 20.f;

	bool m_isCritical = false;

	
	float m_rageTime = 0.f;
	float m_bombTime = 0.f;
	bool m_isRage = false;
	bool m_isBombTime = false;
	shared_ptr<Bomb> m_bomb;

private:
	void InteracitveObjectPick();
	void ProcessGetItem();
	float CalcDamage();
	BulletType CalcBulletType();

public:
	void NetworkSkill(shared_ptr<class SkillPacket> packet);

private:

	void Skill();
	void SkillDealer();
	void SkillHealer();
	void SkillLauncher();
	void SkillTanker();

	void SkillManage();
	shared_ptr<GameObject> m_guardObject;
	shared_ptr<GameObject> m_healObject;
	shared_ptr<GameObject> m_bombObject;
	shared_ptr<GameObject> m_dealObject;

};

