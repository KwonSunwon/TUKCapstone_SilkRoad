#pragma once
#include "PlayerState.h"


class PlayerOnGroundMoveState : public PlayerState
{
public:
	PlayerOnGroundMoveState(shared_ptr<Player> player) : PlayerState(player) {}
	virtual shared_ptr<PlayerState> OnUpdateState() override;
};

class PlayerIdleState : public PlayerOnGroundMoveState
{
public:
	PlayerIdleState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerRunState : public PlayerOnGroundMoveState
{
public:
	PlayerRunState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerWalkState : public PlayerOnGroundMoveState
{
public:
	PlayerWalkState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerFireOnGroundMoveState : public PlayerOnGroundMoveState
{
public:
	PlayerFireOnGroundMoveState(shared_ptr<Player> player) : PlayerOnGroundMoveState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};