#pragma once
#include "UIObject.h"

class TextObject;

class CanvasObject : public UIObject
{
public:
	CanvasObject(float width, float height);
	~CanvasObject() = default;

	void OnResize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseEvent(HWND hWnd);

	virtual void Update();

	void Render2D(const ComPtr<ID2D1DeviceContext2>& device);

	void Add(shared_ptr<UIObject>& uiObject);
	void Add(shared_ptr<TextObject>& textObject);


private:
	vector<shared_ptr<UIObject>>	m_uiObjects;
	vector<shared_ptr<TextObject>>	m_textObjects;

};