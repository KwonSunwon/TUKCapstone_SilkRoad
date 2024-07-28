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
#include "Network.h"
#include "SoundManager.h"

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
		text = L"업그레이드 : '최대 체력 + 10' 구매\n";
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE1)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE1) + 1) * 20);
		break;

	case UPGRADE_MACHINE2:
		text = L"업그레이드 : '기본 공격력 + 2' 구매\n";
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE2)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE2) + 1) * 20);
		break;

	case UPGRADE_MACHINE3:
		text = L"업그레이드 : '공격속도 + 0.2' 구매\n";
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE3)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE3) + 1) * 20);
		break;

	case UPGRADE_MACHINE4:
		text = L"업그레이드 : '크리티컬 데미지 + 5%' 구매\n";
		text += L"보유골드 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetGold()) + L"\n";
		text += L"현재 레벨 : " + to_wstring(GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE4)) + L"\n";
		text += L"구매 비용 : " + to_wstring((GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE4) + 1) * 20);
		break;

	case UPGRADE_MACHINE5:
		text = L"업그레이드 : '체력 재생 + 0.4' 구매\n";
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
	case GAME_START_MONITOR:
		text = (L"게임 시작!");
		break;
	case GAME_HOST_MONITOR:
		text = (L"호스트로 방 개설");
		break;
	case GAME_GUEST_MONITOR:
		text = (L"게스트로 게임 참여");
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
		if (GET_SINGLE(UpgradeManager)->GetGold() < (GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE1) + 1) * 20)
		{
			text->SetText(L"골드가 부족합니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
			break;
		}
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"업그레이드 완료!");
		break;

	case UPGRADE_MACHINE2:
		if (GET_SINGLE(UpgradeManager)->GetGold() < (GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE2) + 1) * 20)
		{
			text->SetText(L"골드가 부족합니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
			break;
		}
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"업그레이드 완료!");
		break;


	case UPGRADE_MACHINE3:
		if (GET_SINGLE(UpgradeManager)->GetGold() < (GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE3) + 1) * 20)
		{
			text->SetText(L"골드가 부족합니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
			break;
		}
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"업그레이드 완료!");
		break;


	case UPGRADE_MACHINE4:
		if (GET_SINGLE(UpgradeManager)->GetGold() < (GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE4) + 1) * 20)
		{
			text->SetText(L"골드가 부족합니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
			break;
		}
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"업그레이드 완료!");
		break;

	case UPGRADE_MACHINE5:
		if (GET_SINGLE(UpgradeManager)->GetGold() < (GET_SINGLE(UpgradeManager)->GetUpgradeLevel(UPGRADE_MACHINE5) + 1) * 20)
		{
			text->SetText(L"골드가 부족합니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
			break;
		}
		GET_SINGLE(UpgradeManager)->Upgrade(m_id);
		text->SetText(L"업그레이드 완료!");
		break;

	case CHARACTER_CHANGER1:
		if (GET_SINGLE(UpgradeManager)->CanClassChange(m_id)) {
			GET_SINGLE(UpgradeManager)->ClassChange(m_id);
			text->SetText(L"'사수' 클래스 변경 완료!");
		}
		else {
			text->SetText(L"이미 누군가 선택한 클래스입니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
		}
		
		break;

	case CHARACTER_CHANGER2:
		if (GET_SINGLE(UpgradeManager)->CanClassChange(m_id)) {
			GET_SINGLE(UpgradeManager)->ClassChange(m_id);
			text->SetText(L"'의무병' 클래스 변경 완료!");
		}
		else {
			text->SetText(L"이미 누군가 선택한 클래스입니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
		}
		break;

	case CHARACTER_CHANGER3:
		if (GET_SINGLE(UpgradeManager)->CanClassChange(m_id)) {
			GET_SINGLE(UpgradeManager)->ClassChange(m_id);
			text->SetText(L"'포병' 클래스 변경 완료!");
		}
		else {
			text->SetText(L"이미 누군가 선택한 클래스입니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
		}
		break;

	case CHARACTER_CHANGER4:
		if (GET_SINGLE(UpgradeManager)->CanClassChange(m_id)) {
			GET_SINGLE(UpgradeManager)->ClassChange(m_id);
			text->SetText(L"'돌격병' 클래스 변경 완료!");
		}
		else {
			text->SetText(L"이미 누군가 선택한 클래스입니다");
			GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_FAIL);
		}
		break;

	case GAME_START_MONITOR:
		GET_SINGLE(SceneManager)->StartNextStage();
		if (GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST) {
			shared_ptr<StageChangePacket> packet = make_shared<StageChangePacket>();
			packet->m_stageIndex = GET_SINGLE(SceneManager)->GetStageIndex();
			SEND(packet);
		}
		text->SetText(L"게임을 시작합니다.");
		break;
	case GAME_HOST_MONITOR:
		GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);
		GET_SINGLE(NetworkManager)->m_networkId = 0;
		GET_SINGLE(NetworkManager)->RunMulti();

		text->SetText(L"방이 개설되었습니다.");
		break;
	case GAME_GUEST_MONITOR:
		GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);
		GET_SINGLE(NetworkManager)->ConnectAsGuest();
		text->SetText(L"호스트의 게임에 참여합니다.");

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


