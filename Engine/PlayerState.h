#pragma once

class Player;

enum class KEY_TYPE;

enum class PLAYER_STATE : uint32
{
	IDLE,
	RUN,
	WALK,
	IDLE_TO_AIM,
	AIM,
	AIM_TO_IDLE,
	FIRE,
	JUMP_UP,
	JUMP_LOOP,
	JUMP_DOWN,
	END
};

class PlayerState
{
public:
	PlayerState(shared_ptr<Player> player) : m_player(player) {}
	virtual ~PlayerState() {}

	virtual shared_ptr<PlayerState> OnUpdateState() { return nullptr; };
	virtual shared_ptr<PlayerState> OnLateUpdateState() { return nullptr; };
	virtual void OnEnter() {};
	virtual void OnExit() {};
protected:
	shared_ptr<Player> m_player;

};