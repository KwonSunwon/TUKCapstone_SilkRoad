#pragma once
#include "MonoBehaviour.h"

class Bomb : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Update() override;

public:
	void SetBombSize(float size) { m_bombSize = size; }
	void SetBombPower(float power) { m_bombPower = power; }
	void SetBombMaxTime(float time) { m_bombMaxTime = time; }
	void SetBombPosition(Vec3 pos) { m_bombPos = pos; }
	void SetBombActive() { m_isBombActivate = true; }

private:
	void explosion();

private:
	float m_bombMaxTime = 1.5f;
	float m_bombTime = 0.f;
	bool m_isBombActivate = false;
	float m_bombSize = 2500.f;
	float m_bombPower = 3000.f;
	Vec3 m_bombPos{ 12500 + 500, -200.f, 15000 + 200 };
	
	

};

