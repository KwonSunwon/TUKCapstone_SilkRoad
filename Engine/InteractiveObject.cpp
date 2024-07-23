#include "pch.h"
#include "InteractiveObject.h"

#include "Transform.h"
#include "RigidBody.h"
#include "Timer.h"
#include "TextObject.h"
#include "UIObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Player.h"


InteractiveObject::InteractiveObject(int num):Component(COMPONENT_TYPE::INTERACTIVE_OBJECT)
{
	m_id = num;
}

void InteractiveObject::Awake()
{
	//SetMonovihaviourName("InteractiveObject");
}
void InteractiveObject::FinalUpdate()
{

}



void InteractiveObject::PrintInteractiveText()
{
	
}

void InteractiveObject::InteractiveFunction()
{


	auto text = make_shared<GettingItemTextObject>(L"Brilliant Behemoth\nAdds explosion to bullets.");


	switch (m_id)
	{
	case UPGRADE_MACHINE1:
		text->SetText(L"UPGRADE_MACHINE1");
		break;

	case UPGRADE_MACHINE2:
		text->SetText(L"UPGRADE_MACHINE2");
		break;


	case UPGRADE_MACHINE3:
		text->SetText(L"UPGRADE_MACHINE3");
		break;


	case UPGRADE_MACHINE4:
		text->SetText(L"UPGRADE_MACHINE4");
		break;

	case UPGRADE_MACHINE5:
		text->SetText(L"UPGRADE_MACHINE5");
		break;

	case CHARACTER_CHANGER1:
		text->SetText(L"CHARACTER_CHANGER1");
		break;

	case CHARACTER_CHANGER2:
		text->SetText(L"CHARACTER_CHANGER2");
		break;

	case CHARACTER_CHANGER3:
		text->SetText(L"CHARACTER_CHANGER3");
		break;

	case CHARACTER_CHANGER4:
		text->SetText(L"CHARACTER_CHANGER4");
		break;

	default:
		break;
	}

	text->SetPivot(ePivot::CENTER);
	text->SetScreenPivot(ePivot::CENTER);
	float yOffset = text->getNum() * 90.f;
	text->SetPosition(Vec2(0.f, yOffset));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddTextObject(text);

}


