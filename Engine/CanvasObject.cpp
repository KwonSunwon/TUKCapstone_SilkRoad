#include "pch.h"
#include "CanvasObject.h"
#include "TextObject.h"
#include "Engine.h"

CanvasObject::CanvasObject(float width, float height) : UIObject{ width, height }
{
	
}

void CanvasObject::OnResize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (auto& ui : m_uiObjects)
		ui->SetPosition(ui->GetPivotPosition());
	for (auto& t : m_textObjects)
		t->SetPosition(t->GetPivotPosition());
}

void CanvasObject::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (auto& ui : m_uiObjects)
		ui->OnMouseEvent(hWnd, message, wParam, lParam);
	for (auto& t : m_textObjects)
		t->OnMouseEvent(hWnd, message, wParam, lParam);
}

void CanvasObject::OnMouseEvent(HWND hWnd)
{
	for (auto& ui : m_uiObjects)
		ui->OnMouseEvent(hWnd);
	for (auto& t : m_textObjects)
		t->OnMouseEvent(hWnd);
}

void CanvasObject::Update()
{
	for (auto& ui : m_uiObjects)
		ui->Update();
	for (auto& t : m_textObjects)
		t->Update();
}

void CanvasObject::Render2D(const ComPtr<ID2D1DeviceContext2>& device)
{
	for (const auto& t : m_textObjects)
		t->Render(device);
}

void CanvasObject::Add(shared_ptr<UIObject>& uiObject)
{
	Vec2 winPos{ GetPosition() };
	float hw{ m_width / 2.0f };
	float hh{ m_height / 2.0f };

	Vec2 uiPos{ uiObject->GetPivotPosition() };
	ePivot uiScreenPivot{ uiObject->GetScreenPivot() };
	uiObject->SetScreenPivot(m_screenPivot);
	switch (uiScreenPivot)
	{
	case ePivot::LEFTTOP:
		uiObject->SetPosition(Vec2{ winPos.x - hw + uiPos.x, winPos.y + hh + uiPos.y });
		break;
	case ePivot::CENTERTOP:
		uiObject->SetPosition(Vec2{ winPos.x + uiPos.x, winPos.y + uiPos.y + hh });
		break;
	case ePivot::RIGHTTOP:
		uiObject->SetPosition(Vec2{ winPos.x + hw + uiPos.x, winPos.y + uiPos.y + hh });
		break;
	case ePivot::LEFTCENTER:
		uiObject->SetPosition(Vec2{ winPos.x - hw + uiPos.x, winPos.y + uiPos.y });
		break;
	case ePivot::CENTER:
		uiObject->SetPosition(Vec2{ winPos.x + uiPos.x, winPos.y + uiPos.y });
		break;
	case ePivot::RIGHTCENTER:
		uiObject->SetPosition(Vec2{ winPos.x + hw + uiPos.x, winPos.y + uiPos.y });
		break;
	case ePivot::LEFTBOT:
		uiObject->SetPosition(Vec2{ winPos.x - hw + uiPos.x, winPos.y - hh + uiPos.y });
		break;
	case ePivot::CENTERBOT:
		uiObject->SetPosition(Vec2{ winPos.x + uiPos.x, winPos.y - hh + uiPos.y });
		break;
	case ePivot::RIGHTBOT:
		uiObject->SetPosition(Vec2{ winPos.x + hw + uiPos.x, winPos.y - hh + uiPos.y });
		break;
	}
	m_uiObjects.push_back(uiObject);
}

void CanvasObject::Add(shared_ptr<TextObject>& textObject)
{
	Vec2 winPos{ GetPosition() };
	float hw{ m_width / 2.0f };
	float hh{ m_height / 2.0f };

	Vec2 textPos{ textObject->GetPivotPosition() };
	ePivot textScreenPivot{ textObject->GetScreenPivot() };
	textObject->SetScreenPivot(ePivot::CENTER);
	switch (textScreenPivot)
	{
	case ePivot::LEFTTOP:
		textObject->SetPosition(Vec2{ winPos.x - hw + textPos.x, winPos.y - hh - textPos.y });
		break;
	case ePivot::CENTERTOP:
		textObject->SetPosition(Vec2{ winPos.x + textPos.x, winPos.y - hh - textPos.y });
		break;
	case ePivot::RIGHTTOP:
		textObject->SetPosition(Vec2{ winPos.x + hw + textPos.x, winPos.y - hh - textPos.y });
		break;
	case ePivot::LEFTCENTER:
		textObject->SetPosition(Vec2{ winPos.x - hw + textPos.x, winPos.y - textPos.y });
		break;
	case ePivot::CENTER:
		textObject->SetPosition(Vec2{ winPos.x + textPos.x, winPos.y - textPos.y });
		break;
	case ePivot::RIGHTCENTER:
		textObject->SetPosition(Vec2{ winPos.x + hw + textPos.x, winPos.y - textPos.y });
		break;
	case ePivot::LEFTBOT:
		textObject->SetPosition(Vec2{ winPos.x - hw + textPos.x, winPos.y + hh - textPos.y });
		break;
	case ePivot::CENTERBOT:
		textObject->SetPosition(Vec2{ winPos.x + textPos.x, winPos.y + hh - textPos.y });
		break;
	case ePivot::RIGHTBOT:
		textObject->SetPosition(Vec2{ winPos.x + hw + textPos.x, winPos.y + hh - textPos.y });
		break;
	}
	m_textObjects.push_back(textObject);
}