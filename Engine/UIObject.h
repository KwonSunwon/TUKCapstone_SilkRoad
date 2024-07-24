#pragma once
#include "MonoBehaviour.h"
enum class ePivot
{
	LEFTTOP, CENTERTOP, RIGHTTOP,
	LEFTCENTER, CENTER, RIGHTCENTER,
	LEFTBOT, CENTERBOT, RIGHTBOT
};

enum class KEY_TYPE;


class Player;
class TextToggleObject;

class UIObject : public MonoBehaviour
{
public:
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseEvent(HWND hWnd);
	virtual void Awake();
	virtual void Update();
	virtual void LateUpdate();

	void SetFitToScreen(bool fitToScreen);
	void SetPosition(const Vec2& position);
	void SetPivot(const ePivot& pivot);
	void SetScreenPivot(const ePivot& pivot);
	void SetScale(const Vec2& scale);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetZOrder(int zOrder);

	ePivot GetPivot() const;
	ePivot GetScreenPivot() const;
	Vec2 GetPivotPosition() const;
	float GetWidth() const;
	float GetHeight() const;
	Vec2 GetPosition() const;
	Vec2 GetScale() const;

protected:
	bool		m_isFitToScreen = false;
	ePivot		m_pivot = ePivot::CENTER;
	ePivot		m_screenPivot = ePivot::CENTER;
	Vec2		m_pivotPosition = Vec2(0.0f,0.0f);
	Vec2		m_scale = Vec2(1.0f, 1.0f);
	Vec2		m_position = Vec2(0.0f, 0.0f);
	float		m_width = 100.f;
	float		m_height = 100.f;
	int			m_zOrder = 10;
};

class UIToggleObject : public UIObject
{
public:
	virtual void Update() override;
	virtual void LateUpdate() override;

	void SetToggleKey(KEY_TYPE key) { m_toggleKey = key; }
	
	void SetToggle(bool toggle) { m_toggle = toggle; }
	bool GetToggle() { return m_toggle; }

private:
	KEY_TYPE m_toggleKey;
	bool m_toggle = false;
};

class PlayerStatUI : public UIToggleObject
{
public:
	virtual void Update() override;
	virtual void LateUpdate() override;

	void AddItemSlot(shared_ptr<UIToggleObject> itemSlot, int index) { m_itemSlots[index] = itemSlot; }

private:
	array<shared_ptr<UIToggleObject>, 17> m_itemSlots;

};