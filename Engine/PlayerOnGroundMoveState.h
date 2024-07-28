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
	float m_footTime = 0.f;
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
	bool m_isFireAnimationAgain = false;	// 공속이 애니메이션 속도보다 빠르면 스테이트 조기 종료 후 idle로 돌아가 다시 발사 준비

};

class PlayerWaitFireOnGroundMoveState : public PlayerOnGroundMoveState
{
public:
	PlayerWaitFireOnGroundMoveState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;


private:
	bool m_isFireAnimationAgain = false;	// 공속이 애니메이션 속도보다 빠르면 스테이트 조기 종료 후 idle로 돌아가 다시 발사 준비

};

class PlayerJumpDownState : public PlayerOnGroundMoveState
{
public:
	PlayerJumpDownState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};