#pragma once
#include "MonoBehaviour.h"

class Enemy : public MonoBehaviour, public std::enable_shared_from_this<Enemy>
{
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	bool GetAttackReady() { return m_fireElapsedTime <= 0.f; }
	void Fire();
	void AddPlayer(shared_ptr<class GameObject> player) { m_players.push_back(player); }
	uint32 GetTargetPlayerIndex() { return m_targetPlayerIndex; }
	void SetTargetPlayerIndex(uint32 index) { m_targetPlayerIndex = index; }
	vector<shared_ptr<GameObject>> GetPlayers() { return m_players; }

	void SetAttackRange(float range) { m_attackRange = range; }
	float GetAttackRange() { return m_attackRange; }

	void SetChaseRange(float range) { m_chaseRange = range; }
	float GetChaseRange() { return m_chaseRange; }

	void SetMaxWalkSpeed(float speed) { m_maxWalkSpeed = speed; }
	float GetMaxWalkSpeed() { return m_maxWalkSpeed; }

	void SetWalkForce(float force) { m_walkForce = force; }
	float GetWalkForce() { return m_walkForce; }

	void SetHP(float hp) { m_HP = hp; }
	float GetHP() { return m_HP; }

	void GetDamage(float damage) { m_HP -= damage; }

	void ProcessPacket(shared_ptr<class EnemyPacket> packet);
	void SetNetworkId(uint32 id) { m_networkId = id; }

private:
	shared_ptr<class EnemyState> m_curState;
	float m_fireRate = 1.f;		// 초당 공격 횟수
	float m_fireElapsedTime = 0.f;
	float m_chaseRange = 6000.f;
	float m_attackRange = 300.f;
	float m_maxWalkSpeed = 800.f;
	float m_walkForce = 300000.f;
	float m_HP = 100.f;

	vector<shared_ptr<class GameObject>> m_players;
	uint32 m_targetPlayerIndex = 0;

	uint32 m_networkId = -1;

};

