#pragma once

enum class ePivot;

class Camera;
class Player;

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

class DebugTextObject : public TextObject
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
};