#pragma once
#include "MonoBehaviour.h"
class TankerSkill : public MonoBehaviour
{
public:
	virtual void Update() override;
	

private:
	bool m_isMakeSound;
	float m_time = 0.f;

};
