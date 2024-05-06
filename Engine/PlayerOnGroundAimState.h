#pragma once
#include "PlayerState.h"

class PlayerOnGroundAimState : public PlayerState
{
public:
	PlayerOnGroundAimState(shared_ptr<Player> player) : PlayerState(player) {}
	virtual shared_ptr<PlayerState> OnUpdateState() override;
private:
	float m_onAirCheckTime = 0.5f;
	float m_onAirTime = 0.f;
};


class PlayerIdleToAimState : public PlayerOnGroundAimState
{
public:
	PlayerIdleToAimState(shared_ptr<Player> player) : PlayerOnGroundAimState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerAimToIdleState : public PlayerOnGroundAimState
{
public:
	PlayerAimToIdleState(shared_ptr<Player> player) : PlayerOnGroundAimState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;

};

class PlayerAimState : public PlayerOnGroundAimState
{
public:
	PlayerAimState(shared_ptr<Player> player) : PlayerOnGroundAimState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;
};

class PlayerFireOnAimState : public PlayerAimState
{
public:
	PlayerFireOnAimState(shared_ptr<Player> player) : PlayerAimState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;

private:
	bool m_isFireAnimationAgain = false;	// ������ �ִϸ��̼� �ӵ����� ������ ������Ʈ ���� ���� �� Aim���� ���ư� �ٽ� �߻� �غ�
	
};