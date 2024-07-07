#include "pch.h"
#include "TextObject.h"
#include "UIObject.h"
#include "Engine.h"
#include "Timer.h"

unordered_map<string, ComPtr<ID2D1SolidColorBrush>>	TextObject::s_brushes;
unordered_map<string, ComPtr<IDWriteTextFormat>>	TextObject::s_formats;

TextObject::TextObject()
{
	m_isValid = true;
	m_isMouseOver =  FALSE ;
	m_rect = {0.0f, 0.0f, static_cast<float>(GEngine->GetWindow().width), static_cast<float>(GEngine->GetWindow().height) };

	m_position = {};
	m_pivotPosition = {};
	m_pivot = { ePivot::CENTER };
	m_screenPivot = { ePivot::CENTER };
	m_width = {};
	m_height = {};
}

void TextObject::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void TextObject::OnMouseEvent(HWND hWnd) { }

void TextObject::Update() { }

void TextObject::Render(const ComPtr<ID2D1DeviceContext2>& device)
{
	device->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));
	device->DrawText(m_text.c_str(), static_cast<UINT32>(m_text.size()), s_formats[m_format].Get(), &m_rect, s_brushes[m_brush].Get());
}

void TextObject::CalcWidthHeight()
{
	if (m_text.empty()) return;

	ComPtr<IDWriteTextLayout> layout;
	GEngine->GetD3D11On12Device()->GetDWriteFactory()->CreateTextLayout(
		m_text.c_str(),
		static_cast<UINT32>(m_text.size()),
		TextObject::s_formats[m_format].Get(),
		static_cast<float>(GEngine->GetWindow().width),
		static_cast<float>(GEngine->GetWindow().height),
		&layout
	);

	DWRITE_TEXT_METRICS metrics;
	layout->GetMetrics(&metrics);

	m_width = metrics.widthIncludingTrailingWhitespace;
	m_height = metrics.height;
}

void TextObject::SetRect(const D2D1_RECT_F& rect)
{
	m_rect = rect;
}

void TextObject::SetBrush(const string& brush)
{
	m_brush = brush;
}

void TextObject::SetFormat(const string& format)
{
	m_format = format;
}


void TextObject::SetText(const wstring& text)
{
	m_text = text;
	if (m_brush.empty()) return;
	if (m_format.empty()) return;

	CalcWidthHeight();
	m_rect = D2D1_RECT_F{ 0.0f, 0.0f, m_width + 1.0f, m_height };
}

void TextObject::SetPivot(const ePivot& pivot)
{
	m_pivot = pivot;
}

void TextObject::SetPosition(const XMFLOAT2& position)
{
	m_pivotPosition = position;
	float width{ static_cast<float>(GEngine->GetWindow().width) }, height{ static_cast<float>(GEngine->GetWindow().height) };

	switch (m_screenPivot)
	{
	case ePivot::LEFTTOP:
		m_position.x = position.x;
		m_position.y = position.y;
		break;
	case ePivot::CENTERTOP:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y;
		break;
	case ePivot::RIGHTTOP:
		m_position.x = position.x + width;
		m_position.y = position.y;
		break;
	case ePivot::LEFTCENTER:
		m_position.x = position.x;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::CENTER:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::RIGHTCENTER:
		m_position.x = position.x + width;
		m_position.y = position.y + height / 2.0f;
		break;
	case ePivot::LEFTBOT:
		m_position.x = position.x;
		m_position.y = position.y + height;
		break;
	case ePivot::CENTERBOT:
		m_position.x = position.x + width / 2.0f;
		m_position.y = position.y + height;
		break;
	case ePivot::RIGHTBOT:
		m_position.x = position.x + width;
		m_position.y = position.y + height;
		break;
	}

	switch (m_pivot)
	{
	case ePivot::LEFTTOP:
		break;
	case ePivot::CENTERTOP:
		m_position.x -= m_width / 2.0f;
		break;
	case ePivot::RIGHTTOP:
		m_position.x -= m_width;
		break;
	case ePivot::LEFTCENTER:
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::CENTER:
		m_position.x -= m_width / 2.0f;
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::RIGHTCENTER:
		m_position.x -= m_width;
		m_position.y -= m_height / 2.0f;
		break;
	case ePivot::LEFTBOT:
		m_position.y -= m_height;
		break;
	case ePivot::CENTERBOT:
		m_position.x -= m_width / 2.0f;
		m_position.y -= m_height;
		break;
	case ePivot::RIGHTBOT:
		m_position.x -= m_width;
		m_position.y -= m_height;
		break;
	}
}

void TextObject::SetScreenPivot(const ePivot& pivot)
{
	m_screenPivot = pivot;
}

bool TextObject::isValid() const
{
	return m_isValid;
}

wstring TextObject::GetText() const
{
	return m_text;
}

ePivot TextObject::GetPivot() const
{
	return m_pivot;
}

ePivot TextObject::GetScreenPivot() const
{
	return m_screenPivot;
}

Vec2 TextObject::GetPosition() const
{
	return m_position;
}

Vec2 TextObject::GetPivotPosition() const
{
	return m_pivotPosition;
}

float TextObject::GetWidth() const
{
	return m_width;
}

float TextObject::GetHeight() const
{
	return m_height;
}

GettingItemTextObject::GettingItemTextObject(const wstring& itemName)
{
	m_timer = 0.0f;
	m_alpha = 1.0f;


	SetBrush("WHITE");
	SetFormat("default");
	SetText(itemName);
	SetPivot(ePivot::CENTER);
	SetScreenPivot(ePivot::CENTER);
	SetPosition({ 0.0f, 100.0f });
}

void GettingItemTextObject::Render(const ComPtr<ID2D1DeviceContext2>& device)
{
	device->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));
	s_brushes[m_brush]->SetOpacity(m_alpha);
	device->DrawText(m_text.c_str(), static_cast<UINT32>(m_text.size()), s_formats[m_format].Get(), &m_rect, s_brushes[m_brush].Get());
	s_brushes[m_brush]->SetOpacity(1.0f);
}

void GettingItemTextObject::Update()
{
	m_timer += DELTA_TIME;
	m_alpha = min(1.0f, 3.0f - m_timer);

	if (m_timer > 3.0f)
	{
		m_timer = 0.0f;
		m_isValid = false;
	}
}