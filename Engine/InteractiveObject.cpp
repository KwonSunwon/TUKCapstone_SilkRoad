#include "pch.h"
#include "InteractiveObject.h"

#include "Transform.h"
#include "RigidBody.h"
#include "Timer.h"
#include "TextObject.h"
#include "UIObject.h"
#include "Scene.h"
#include "SceneManager.h"


InteractiveObject::InteractiveObject():Component(COMPONENT_TYPE::INTERACTIVE_OBJECT)
{
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
	return;
	auto text = make_shared<GettingItemTextObject>(L"Press \'G\' To Change Stage");
	text->SetPivot(ePivot::CENTER);
	text->SetScreenPivot(ePivot::CENTER);
	float yOffset = text->getNum() * 90.f;
	text->SetPosition(Vec2(0.f, yOffset));
	GET_SINGLE(SceneManager)->GetActiveScene()->AddTextObject(text);
	
}

void InteractiveObject::InteractiveFunction()
{
}


