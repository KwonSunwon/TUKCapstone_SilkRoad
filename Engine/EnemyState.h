#pragma once

class Enemy;

enum class KEY_TYPE;

enum class ENEMY_STATE : uint32
{
	IDLE,
	WALK,
	ATTACK,
	DIE,
	END
};

class EnemyState
{
public:
	EnemyState(shared_ptr<Enemy> enemy) : m_enemy(enemy) {}
	virtual ~EnemyState() {}

	virtual shared_ptr<EnemyState> OnUpdateState() { return nullptr; };
	virtual shared_ptr<EnemyState> OnLateUpdateState() { return nullptr; };
	virtual void OnEnter() {};
	virtual void OnExit() {};
protected:
	shared_ptr<Enemy> m_enemy;

};

class EnemyIdleState : public EnemyState
{
public:
	EnemyIdleState(shared_ptr<Enemy> enemy) : EnemyState(enemy) {}

	virtual shared_ptr<EnemyState> OnUpdateState() override;
	virtual void OnEnter() override;

};

class EnemyWalkState : public EnemyState
{
public:
	EnemyWalkState(shared_ptr<Enemy> enemy) : EnemyState(enemy) {}
	virtual shared_ptr<EnemyState> OnUpdateState();
	virtual void OnEnter() override;
};

class EnemyAttackState : public EnemyState
{
public:
	EnemyAttackState(shared_ptr<Enemy> enemy) : EnemyState(enemy) {}
	virtual shared_ptr<EnemyState> OnUpdateState();
	virtual shared_ptr<EnemyState> OnLateUpdateState();

	virtual void OnEnter() override;
};

class EnemyDieState : public EnemyState
{
public:
	EnemyDieState(shared_ptr<Enemy> enemy) : EnemyState(enemy) {}
};