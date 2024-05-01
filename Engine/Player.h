#pragma once
#include "MonoBehaviour.h"


class Player : public MonoBehaviour, public std::enable_shared_from_this<Player>
{
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	shared_ptr<class PlayerState> m_curState;
};

