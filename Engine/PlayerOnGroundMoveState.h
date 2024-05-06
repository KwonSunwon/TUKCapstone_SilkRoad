#pragma once
#include "PlayerState.h"


class PlayerOnGroundMoveState : public PlayerState
{
public:
	PlayerOnGroundMoveState(shared_ptr<Player> player) : PlayerState(player) {}
	virtual shared_ptr<PlayerState> OnUpdateState() override;

private:
	float m_onAirCheckTime = 0.5f;
	float m_onAirTime = 0.f;
};

class PlayerWalkState : public PlayerOnGroundMoveState
{
public:
	PlayerWalkState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerFireOnGroundMoveState : public PlayerOnGroundMoveState
{
public:
	PlayerFireOnGroundMoveState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;

private:
	bool m_isFireAnimationAgain = false;	// ������ �ִϸ��̼� �ӵ����� ������ ������Ʈ ���� ���� �� idle�� ���ư� �ٽ� �߻� �غ�

};

class PlayerWaitFireOnGroundMoveState : public PlayerOnGroundMoveState
{
public:
	PlayerWaitFireOnGroundMoveState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;


private:
	bool m_isFireAnimationAgain = false;	// ������ �ִϸ��̼� �ӵ����� ������ ������Ʈ ���� ���� �� idle�� ���ư� �ٽ� �߻� �غ�

};

class PlayerJumpDownState : public PlayerOnGroundMoveState
{
public:
	PlayerJumpDownState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};