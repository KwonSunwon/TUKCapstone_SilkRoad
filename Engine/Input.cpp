#include "pch.h"
#include "Input.h"
#include "Engine.h"

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
}