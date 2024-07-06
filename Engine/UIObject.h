#pragma once

enum class ePivot
{
	LEFTTOP, CENTERTOP, RIGHTTOP,
	LEFTCENTER, CENTER, RIGHTCENTER,
	LEFTBOT, CENTERBOT, RIGHTBOT
};

class UIObject
{
public:
	UIObject(float width, float height);
	virtual ~UIObject() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseEvent(HWND hWnd);
	virtual void Update();

	void SetFitToScreen(bool fitToScreen);
	void SetPosition(const Vec2& position);
	void SetPivot(const ePivot& pivot);
	void SetScreenPivot(const ePivot& pivot);
	void SetScale(const Vec2& scale);
	void SetWidth(float width);
	void SetHeight(float height);

	ePivot GetPivot() const;
	ePivot GetScreenPivot() const;
	Vec2 GetPivotPosition() const;
	float GetWidth() const;
	float GetHeight() const;
	Vec2 GetPosition() const;

protected:
	bool		m_isFitToScreen;
	ePivot		m_pivot;
	ePivot		m_screenPivot;
	Vec2		m_pivotPosition;
	Vec2		m_scale;
	Vec2		m_position;
	float		m_width;
	float		m_height;
};