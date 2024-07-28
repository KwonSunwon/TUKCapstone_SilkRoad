#pragma once

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	SPACE = VK_SPACE,

	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',
	P = 'P',
	F = 'F',

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',

	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,

	TAB = VK_TAB,

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12,

	ENTER = VK_RETURN,

	ESC = VK_ESCAPE,
};

static constexpr int NUM_OF_KEYS = 36;

constexpr std::array<KEY_TYPE, NUM_OF_KEYS> ALL_KEYS =
{
	KEY_TYPE::UP,
	KEY_TYPE::DOWN,
	KEY_TYPE::LEFT,
	KEY_TYPE::RIGHT,
	KEY_TYPE::W,
	KEY_TYPE::A,
	KEY_TYPE::S,
	KEY_TYPE::D,
	KEY_TYPE::Q,
	KEY_TYPE::E,
	KEY_TYPE::Z,
	KEY_TYPE::C,
	KEY_TYPE::P,
	KEY_TYPE::F,
	KEY_TYPE::KEY_1,
	KEY_TYPE::KEY_2,
	KEY_TYPE::KEY_3,
	KEY_TYPE::KEY_4,
	KEY_TYPE::KEY_5,
	KEY_TYPE::LBUTTON,
	KEY_TYPE::RBUTTON,
	KEY_TYPE::SPACE,
	KEY_TYPE::TAB,
	KEY_TYPE::F1,
	KEY_TYPE::F2,
	KEY_TYPE::F3,
	KEY_TYPE::F5,
	KEY_TYPE::F6,
	KEY_TYPE::F7,
	KEY_TYPE::F8,
	KEY_TYPE::F9,
	KEY_TYPE::F10,
	KEY_TYPE::F11,
	KEY_TYPE::F12,
	KEY_TYPE::ENTER,
	KEY_TYPE::ESC,

};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hwnd);
	void Update();

	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	bool GetAnyButtonDown();

	const POINT& GetMousePos() { return m_mousePos; }
	const POINT& GetMouseDelta() { return m_mouseDelta; }

	Vec2 m_screenCenter{};

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_states[static_cast<uint8>(key)]; }

private:
	HWND m_hwnd;
	vector<KEY_STATE> m_states;
	POINT m_mousePos = {};
	POINT m_mouseLastPos = {};
	POINT m_mouseDelta = {};
};

