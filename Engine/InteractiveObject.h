/*
상호작용 가능한 객체에 추가할 컴포넌트
공통적으로 scene에서 상호작용 객체로 분류되어 매 프레임 피킹, 조건 적용
각 객체는 이를 상속받아 개인적인 기능 수행
*/

#pragma once
#include "Component.h"
class InteractiveObject : public Component
{

public :
	InteractiveObject();

	virtual void Awake() override;
	//virtual void Update() override;
	virtual void FinalUpdate() override;

public:
	void PrintInteractiveText();
	void InteractiveFunction();

private:
	float m_timer = 0.f;

};


