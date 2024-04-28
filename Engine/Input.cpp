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
	if (m_hwnd != hwnd) {
		for (const KEY_TYPE key : ALL_KEYS)
			m_states[static_cast<int32>(key)] = KEY_STATE::NONE;
		return;
	}

	if (GetButton(KEY_TYPE::ESC))
		PostQuitMessage(0);

	for (const KEY_TYPE key : ALL_KEYS) {
		// 키가 눌려 있으면 true
		if (::GetAsyncKeyState(static_cast<int32>(key)) & 0x8000) {
			KEY_STATE& state = m_states[static_cast<int32>(key)];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else {
			KEY_STATE& state = m_states[static_cast<int32>(key)];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}
	SetCursor(NULL);
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);

	m_mouseDelta.x = (double)(ptCursorPos.x - 300);
	m_mouseDelta.y = (double)(ptCursorPos.y - 300);
	SetCursorPos(300, 300);
}