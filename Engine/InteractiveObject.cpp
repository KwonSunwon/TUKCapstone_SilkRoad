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
#include "UpgradeManager.h"


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
	wstring text{};
	
	switch (m_id)
	{
	case UPGRADE_MACHINE1:
		text = L"'업그레이드 1' 구매\n";
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE1)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE1) + 1) * 20);
		break;

	case UPGRADE_MACHINE2:
		text = (L"'업그레이드 2' 구매\n");
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE2)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE2) + 1) * 20);
		break;

	case UPGRADE_MACHINE3:
		text = (L"'업그레이드 3' 구매\n");
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE3)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE3) + 1) * 20);
		break;

	case UPGRADE_MACHINE4:
		text = (L"'업그레이드 4' 구매\n");
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE4)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE4) + 1) * 20);
		break;

	case UPGRADE_MACHINE5:
		text = (L"'업그레이드 5' 구매\n");
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE5)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE5) + 1) * 20);
		break;

	case CHARACTER_CHANGER1:
		text = (L"'사수'로 클래스 변경");
		break;

	case CHARACTER_CHANGER2:
		text = (L"'의무병'으로 클래스 변경");
		break;

	case CHARACTER_CHANGER3:
		text = (L"'포병'으로 클래스 변경");
		break;
	case CHARACTER_CHANGER4:
		text = (L"'돌격병'으로 클래스 변경");
		break;

	default:
		break;
	}

	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetText(text);
	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetPosition(Vec2(0.f, -30.f));
	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetVisible(true);
}

void InteractiveObject::InteractiveFunction()
{


	auto text = make_shared<GettingItemTextObject>(L"Brilliant Behemoth\nAdds explosion to bullets.");


	switch (m_id)
	{
	case UPGRADE_MACHINE1:
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"'업그레이드 1' 구매 완료!");
		break;

	case UPGRADE_MACHINE2:
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"'업그레이드 2' 구매 완료!");
		break;


	case UPGRADE_MACHINE3:
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"'업그레이드 3' 구매 완료!");
		break;


	case UPGRADE_MACHINE4:
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"'업그레이드 4' 구매 완료!");
		break;

	case UPGRADE_MACHINE5:
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"'업그레이드 5' 구매 완료!");
		break;

	case CHARACTER_CHANGER1:
		GET_SINGLE(UpgradeManager)->ClassChange(m_id);
		text->SetText(L"'사수' 클래스 변경 완료!");
		break;

	case CHARACTER_CHANGER2:
		GET_SINGLE(UpgradeManager)->ClassChange(m_id);
		text->SetText(L"'의무병' 클래스 변경 완료!");
		break;

	case CHARACTER_CHANGER3:
		GET_SINGLE(UpgradeManager)->ClassChange(m_id);
		text->SetText(L"'포병' 클래스 변경 완료!");
		break;

	case CHARACTER_CHANGER4:
		GET_SINGLE(UpgradeManager)->ClassChange(m_id);
		text->SetText(L"'돌격병' 클래스 변경 완료!");
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


