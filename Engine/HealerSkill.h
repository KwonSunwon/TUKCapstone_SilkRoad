#pragma once
#include "MonoBehaviour.h"
class HealerSkill : public MonoBehaviour
{
public:
	virtual void Update() override;


private:
	bool m_isMakeSound;
	float m_time = 0.f;

};
