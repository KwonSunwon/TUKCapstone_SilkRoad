#pragma once
#include "PlayerState.h"

class PlayerOnAirState : public PlayerState
{
public:
	PlayerOnAirState(shared_ptr<Player> player) : PlayerState(player) {}
	virtual shared_ptr<PlayerState> OnUpdateState() override;
};

class PlayerJumpUpState : public PlayerOnAirState
{
public:
	PlayerJumpUpState(shared_ptr<Player> player) : PlayerOnAirState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnLateUpdateState() override;
};

class PlayerJumpLoopState : public PlayerOnAirState
{
public:
	PlayerJumpLoopState(shared_ptr<Player> player) : PlayerOnAirState(player) {}

	virtual void OnEnter() override;
	virtual shared_ptr<PlayerState> OnUpdateState() override;
};