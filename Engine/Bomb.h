#pragma once
#include "MonoBehaviour.h"

class Bomb : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Update() override;

private:
	void explosion();

private:
	float bombTime = 0.f;
	bool isBombActivate = false;

};

