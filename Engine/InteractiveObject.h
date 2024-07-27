/*
상호작용 가능한 객체에 추가할 컴포넌트
공통적으로 scene에서 상호작용 객체로 분류되어 매 프레임 피킹, 조건 적용
각 객체는 이를 상속받아 개인적인 기능 수행
*/

#pragma once
#include "Component.h"
#include "TextObject.h"


enum EnumInteract
{
	UPGRADE_MACHINE1,
	UPGRADE_MACHINE2,
	UPGRADE_MACHINE3,
	UPGRADE_MACHINE4,
	UPGRADE_MACHINE5,
	CHARACTER_CHANGER1,
	CHARACTER_CHANGER2,
	CHARACTER_CHANGER3,
	CHARACTER_CHANGER4,
	STAGE_PORTAL,
	INTERACT_TYPE_COUNT
};


class InteractiveObject : public Component
{

public :
	InteractiveObject(int num);

	virtual void Awake() override;
	//virtual void Update() override;
	virtual void FinalUpdate() override;

public:
	virtual void PrintInteractiveText();
	virtual void InteractiveFunction();

protected:
	float m_timer = 0.f;
	shared_ptr<TextObject> m_textObject;
	int m_id = -1;
};


