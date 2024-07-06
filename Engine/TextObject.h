#pragma once

enum class ePivot;

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