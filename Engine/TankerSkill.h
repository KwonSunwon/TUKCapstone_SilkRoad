#pragma once
#include "MonoBehaviour.h"
class TankerSkill : public MonoBehaviour
{
public:
	virtual void Awake() override;
	virtual void Update() override;
	float m_time = 0.f;

private:
	bool m_isMakeSound;
	

};
