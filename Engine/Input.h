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

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',

	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,

	ESC = VK_ESCAPE,
};

static constexpr int NUM_OF_KEYS = 22;

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
	KEY_TYPE::KEY_1,
	KEY_TYPE::KEY_2,
	KEY_TYPE::KEY_3,
	KEY_TYPE::KEY_4,
	KEY_TYPE::KEY_5,
	KEY_TYPE::LBUTTON,
	KEY_TYPE::RBUTTON,
	KEY_TYPE::SPACE,
	KEY_TYPE::ESC,
	KEY_TYPE::P,

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

