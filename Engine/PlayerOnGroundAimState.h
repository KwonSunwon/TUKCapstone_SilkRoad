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
	bool m_isFireAnimationAgain = false;	// 공속이 애니메이션 속도보다 빠르면 스테이트 조기 종료 후 Aim으로 돌아가 다시 발사 준비
	
};