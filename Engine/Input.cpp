#include "pch.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Packet.h"
#include "Network.h"
#include "UpgradeManager.h"


void Input::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
	ShowCursor(FALSE);
}

void Input::Update()
{
	HWND hwnd = ::GetActiveWindow();
	if(m_hwnd != hwnd) {
		for(const KEY_TYPE key : ALL_KEYS)
			m_states[static_cast<int32>(key)] = KEY_STATE::NONE;
		return;
	}

	if(GetButton(KEY_TYPE::ESC))
		PostQuitMessage(0);

	for(const KEY_TYPE key : ALL_KEYS) {
		// 키가 눌려 있으면 true
		if(::GetAsyncKeyState(static_cast<int32>(key)) & 0x8000) {
			KEY_STATE& state = m_states[static_cast<int32>(key)];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if(state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else {
			KEY_STATE& state = m_states[static_cast<int32>(key)];

			// 이전 프레임에 키를 누른 상태라면 UP
			if(state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}
	SetCursor(NULL);
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);

	// 화면 중앙을 기준으로 마우스 위치를 계산
	RECT rcWindow;
	GetWindowRect(hwnd, &rcWindow);
	double centerX = (rcWindow.right - rcWindow.left) / 2 + rcWindow.left;
	double centerY = (rcWindow.bottom - rcWindow.top) / 2 + rcWindow.top;
	m_screenCenter = Vec2(centerX, centerY);

	m_mouseDelta.x = (double)(ptCursorPos.x - centerX);
	m_mouseDelta.y = (double)(ptCursorPos.y - centerY);

	SetCursorPos(centerX, centerY);


	// 테스트용 키
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_4))
	{
		GET_SINGLE(SceneManager)->ResetStage();
	}
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_5))
	{
		GET_SINGLE(SceneManager)->StartNextStage();
		if (GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST) {
			shared_ptr<StageChangePacket> packet = make_shared<StageChangePacket>();
			packet->m_stageIndex = GET_SINGLE(SceneManager)->GetStageIndex();
			SEND(packet);
		}

	}
	if (INPUT->GetButtonDown(KEY_TYPE::E))
	{
		int chaClass = GET_SINGLE(UpgradeManager)->GetClass() + 1;
		if (chaClass == 9) chaClass = 5;

		GET_SINGLE(UpgradeManager)->ClassChange(chaClass);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::Z))
	{

		GET_SINGLE(UpgradeManager)->AddGold(100.f);
	}
}

bool Input::GetAnyButtonDown()
{
	for (const KEY_TYPE key : ALL_KEYS)
	{
		if (GetButtonDown(key))
			return true;
	}
	return false;
}