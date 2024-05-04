#pragma once
#include "MonoBehaviour.h"

class PlayerBullet : public MonoBehaviour
{
public:
	virtual void Update() override;
	void Fire(class shared_ptr<class Player> shooter);
};