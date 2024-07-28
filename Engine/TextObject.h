#pragma once

enum class ePivot;
enum class KEY_TYPE;

class Camera;
class Player;
class Light;

class TextObject
{
public:
	TextObject();
	~TextObject() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseEvent(HWND hWnd);

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device);
	virtual void Update();

	void Delete();

	void CalcWidthHeight();
	void SetRect(const D2D1_RECT_F& rect);
	void SetBrush(const string& brush);
	void SetFormat(const string& format);
	void SetText(const wstring& text);
	void SetPivot(const ePivot& pivot);
	void SetScreenPivot(const ePivot& pivot);
	void SetPosition(const XMFLOAT2& position);

	bool isValid() const;
	wstring GetText() const;
	ePivot GetPivot() const;
	ePivot GetScreenPivot() const;
	Vec2 GetPosition() const;
	Vec2 GetPivotPosition() const;
	float GetWidth() const;
	float GetHeight() const;

	static unordered_map<string, ComPtr<ID2D1SolidColorBrush>>	s_brushes;
	static unordered_map<string, ComPtr<IDWriteTextFormat>>		s_formats;

protected:
	bool		m_isValid;
	bool		m_isMouseOver;

	D2D1_RECT_F	m_rect;
	string		m_brush;
	string		m_format;
	wstring		m_text;

	ePivot		m_pivot;
	ePivot		m_screenPivot;
	Vec2		m_position;
	Vec2		m_pivotPosition;
	float		m_width;
	float		m_height;
};

class GettingItemTextObject : public TextObject
{
public:
	GettingItemTextObject(const wstring& itemName);
	~GettingItemTextObject() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;
	virtual void Update() override;

	int getNum() { return s_num; }
	
private:
	float m_timer;
	float m_alpha;

	static int s_num;

};

class DamageIndicatorTextObject : public TextObject
{
public:
	DamageIndicatorTextObject(const wstring& damage);
	~DamageIndicatorTextObject() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;
	virtual void Update() override;

	void SetOriginPosition(const Vec3& position) { m_originPosition = position; }

private:
	shared_ptr<Camera> m_camera;
	Vec3 m_originPosition;

	float m_timer;
	float m_alpha;
	float m_scale;

	bool m_isOnScreen;
};

class TextToggleObject : public TextObject
{
public:
	TextToggleObject();
	~TextToggleObject() = default;

	virtual void Update() override;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;

	void SetToggle(bool toggle) { m_toggle = toggle; }
	bool GetToggle() { return m_toggle; }
	void SetToggleKey(KEY_TYPE key) { m_toggleKey = key; }

private:
	bool m_toggle;
	KEY_TYPE m_toggleKey;

};

class DebugTextObject : public TextToggleObject
{
public:
	DebugTextObject();
	~DebugTextObject() = default;

	//virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;
	virtual void Update() override;

	void setPlayer(shared_ptr<Player> const player) { m_player = player; }

private:
	shared_ptr<Player> m_player;
	shared_ptr<Camera> m_camera;
	shared_ptr<Light> m_light;
};

class HPTextObject : public TextObject
{
public:
	HPTextObject();
	~HPTextObject() = default;

	virtual void Update() override;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;

private:
	float m_displayHP;
	bool m_timerOn;
	float m_timer;
	float m_scale;

};

class PlayerStatTextObject : public TextToggleObject
{
public:
	PlayerStatTextObject();
	~PlayerStatTextObject() = default;

	virtual void Update() override;
};

class PlayerClassTextObject : public TextToggleObject
{
public:
	PlayerClassTextObject();
	~PlayerClassTextObject() = default;

	virtual void Update() override;
};

class OutgameUpgradeTextObject : public TextToggleObject
{
public:
	OutgameUpgradeTextObject();
	~OutgameUpgradeTextObject() = default;

	virtual void Update() override;
};

class InteractiveObjectText : public TextObject
{
public:
	InteractiveObjectText();
	~InteractiveObjectText() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;

	void SetVisible(bool visible) { m_isVisible = visible; }
	void SetInteractType(int type) { m_interactType = type; }

private:
	bool m_isVisible = false;
	int m_interactType;
};

class SkillCoolTimeTextObject : public TextObject
{
public:
	SkillCoolTimeTextObject();
	~SkillCoolTimeTextObject() = default;

	virtual void Update() override;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& device) override;

private:
	wstring m_skillReamainText;
};

class NetworkPlayerHPTextObject : public TextToggleObject
{
public:
	NetworkPlayerHPTextObject();
	~NetworkPlayerHPTextObject() = default;

	virtual void Update() override;

	void SetNetworkPlayerIdx(int idx) { m_networkPlayerIdx = idx; }

private:
	int m_networkPlayerIdx;
};

class PlayTimeTextObject : public TextObject
{
public:
	PlayTimeTextObject();
	~PlayTimeTextObject() = default;

	virtual void Update() override;
};

class DifficultyTextObject : public TextObject
{
public:
	DifficultyTextObject();
	~DifficultyTextObject() = default;

	virtual void Update() override;
};

class DifficultyInfoTextObject : public TextObject
{
public:
	DifficultyInfoTextObject();
	~DifficultyInfoTextObject() = default;

	virtual void Update() override;
};

class GameOverTextObject : public TextToggleObject
{
public:
	GameOverTextObject();
	~GameOverTextObject() = default;

	virtual void Update() override;
};