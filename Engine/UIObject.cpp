#include "pch.h"
#include "UIObject.h"
#include "Engine.h"

UIObject::UIObject(float width, float height)
{
	m_width = width;
	m_height = height;
	m_isFitToScreen = false;
	m_pivot = ePivot::CENTER;
	m_screenPivot = ePivot::CENTER;
	m_pivotPosition = Vec2(0.0f, 0.0f);
	m_scale = Vec2(1.0f, 1.0f);
}

void UIObject::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void UIObject::OnMouseEvent(HWND hWnd) { }

void UIObject::Update()
{

}


void UIObject::SetFitToScreen(bool fitToScreen)
{
	m_isFitToScreen = fitToScreen;
}

void UIObject::SetPosition(const Vec2& position)
{
	m_pivotPosition = position;
	float width{ static_cast<float>(GEngine->GetWindow().width) }, height{ static_cast<float>(GEngine->GetWindow().height) };
	switch (m_screenPivot)
	{
	case ePivot::LEFTTOP:
		m_position.x = position.x - width / 2.0f;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::CENTERTOP:
		m_position.x = position.x;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::RIGHTTOP:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::LEFTCENTER:
		m_position.x = position.x - width / 2.0f;
		m_position.y = position.y;
		break;
	case ePivot::CENTER:
		m_position.x = position.x;
		m_position.y = position.y;
		break;
	case ePivot::RIGHTCENTER:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y;
		break;
	case ePivot::LEFTBOT:
		m_position.x = position.x - width / 2.0f;
		m_position.y = position.y - height / 2.0f;
		break;
	case ePivot::CENTERBOT:
		m_position.x = position.x;
		m_position.y = position.y - height / 2.0f;
		break;
	case ePivot::RIGHTBOT:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y - height / 2.0f;
		break;
	}

	switch (m_pivot)
	{
	case ePivot::LEFTTOP:
		m_position.x += m_width / 2.0f;
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::CENTERTOP:
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::RIGHTTOP:
		m_position.x -= m_width / 2.0f;
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::LEFTCENTER:
		m_position.x += m_width / 2.0f;
		break;
	case ePivot::CENTER:
		break;
	case ePivot::RIGHTCENTER:
		m_position.x -= m_width / 2.0f;
		break;
	case ePivot::LEFTBOT:
		m_position.x += m_width / 2.0f;
		m_position.y += m_height / 2.0f;
		break;
	case ePivot::CENTERBOT:
		m_position.y += m_height / 2.0f;
		break;
	case ePivot::RIGHTBOT:
		m_position.x -= m_width / 2.0f;
		m_position.y += m_height / 2.0f;
		break;
	}

}

void UIObject::SetPivot(const ePivot& pivot)
{
	m_pivot = pivot;
}

void UIObject::SetScreenPivot(const ePivot& pivot)
{
	m_screenPivot = pivot;
}

void UIObject::SetScale(const Vec2& scale)
{
	m_scale = scale;
}

void UIObject::SetWidth(float width)
{
	float deltaWidth{ width - m_width };
	m_width = width;

	switch (m_pivot)
	{
	case ePivot::LEFTTOP:
	case ePivot::LEFTCENTER:
	case ePivot::LEFTBOT:
		m_position.x += deltaWidth / 2.0f;
		break;
	case ePivot::RIGHTTOP:
	case ePivot::RIGHTCENTER:
	case ePivot::RIGHTBOT:
		m_position.x -= deltaWidth / 2.0f;
		break;
	}
}

void UIObject::SetHeight(float height)
{
	float deltaHeight{ height - m_height };
	m_height = height;

	switch (m_pivot)
	{
	case ePivot::LEFTTOP:
	case ePivot::RIGHTTOP:
	case ePivot::CENTERTOP:
		m_position.y += deltaHeight / 2.0f;
		break;
	case ePivot::LEFTBOT:
	case ePivot::CENTERBOT:
	case ePivot::RIGHTBOT:
		m_position.y -= deltaHeight / 2.0f;
		break;
	}
}


ePivot UIObject::GetPivot() const
{
	return m_pivot;
}

ePivot UIObject::GetScreenPivot() const
{
	return m_screenPivot;
}

Vec2 UIObject::GetPivotPosition() const
{
	return m_pivotPosition;
}

float UIObject::GetWidth() const
{
	return m_width;
}

float UIObject::GetHeight() const
{
	return m_height;
}

Vec2 UIObject::GetPosition() const
{
	return m_position;
}