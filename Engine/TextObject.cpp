#include "pch.h"
#include "TextObject.h"
#include "UIObject.h"
#include "Engine.h"
#include "Timer.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Player.h"
#include "RigidBody.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
#include "GameObject.h"
#include "Input.h"
#include "Player.h"

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

int GettingItemTextObject::s_num = 0;

GettingItemTextObject::GettingItemTextObject(const wstring& itemName)
{
	m_timer = 0.0f;
	m_alpha = 1.0f;
	s_num++;

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
	const float lifeTime{ 3.0f };
	m_timer += DELTA_TIME;
	m_alpha = min(1.0f, lifeTime - m_timer);

	if (m_timer > lifeTime)
	{
		m_timer = 0.0f;
		m_isValid = false;
		s_num--;
	}
}

DamageIndicatorTextObject::DamageIndicatorTextObject(const wstring& damage)
{
	m_timer = 0.0f;
	m_alpha = 1.0f;
	m_camera = GET_SINGLE(SceneManager) ->GetActiveScene()->GetMainCamera();
	m_isOnScreen = false;
	m_scale = 0.8f;

	SetBrush("YELLOW");
	SetFormat("default");
	SetText(damage);
	SetPivot(ePivot::CENTER);
	SetScreenPivot(ePivot::LEFTTOP);
}

void DamageIndicatorTextObject::Render(const ComPtr<ID2D1DeviceContext2>& device)
{
	if (!m_isOnScreen) return;

	D2D1::Matrix3x2F matrix{ D2D1::Matrix3x2F::Identity() };
	matrix.SetProduct(matrix, D2D1::Matrix3x2F::Scale(m_scale, m_scale, D2D1_POINT_2F{ m_rect.right / 2.0f, m_rect.bottom / 2.0f }));
	matrix.SetProduct(matrix, D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));
	device->SetTransform(matrix);

	s_brushes[m_brush]->SetOpacity(m_alpha);
	device->DrawText(m_text.c_str(), static_cast<UINT32>(m_text.size()), s_formats[m_format].Get(), &m_rect, s_brushes[m_brush].Get());
	s_brushes[m_brush]->SetOpacity(1.0f);

}

void DamageIndicatorTextObject::Update()
{
	const float lifeTime{ 1.5f };

	Vec3 screenPos{ m_originPosition };
	screenPos = Vec3::Transform(screenPos, m_camera->GetViewMatrix());
	screenPos = Vec3::Transform(screenPos, m_camera->GetProjectionMatrix());

	if (screenPos.x < -1.0f || screenPos.x > 1.0f || screenPos.y < -1.0f || screenPos.y > 1.0f || screenPos.z < 0.0f || screenPos.z > 1.0f)
	{
		m_isOnScreen = false;
	}
	else
	{
		screenPos.x = GEngine->GetWindow().width * (screenPos.x + 1.0f) / 2.0f;
		screenPos.y = GEngine->GetWindow().height * (1.0f - screenPos.y) / 2.0f;
		screenPos.y -= GEngine->GetWindow().height * 0.03f * m_timer;

		SetPosition(Vec2{ screenPos.x, screenPos.y });
		m_isOnScreen = true;
	}

	m_alpha = min(1.0f, lifeTime - m_timer);

	float angle{ 30.0f + 150.0f * (m_timer / 0.2f) };
	m_scale = 1.0f + sinf(XMConvertToRadians(angle)) * 0.5f;
	if (m_timer >= 0.2f)
		m_scale = 1.0f;
	if (m_brush == "YELLOW")
		m_scale += 0.3f;

	m_timer += DELTA_TIME;

	if (m_timer > lifeTime)
	{
		m_isValid = false;
	}
}

TextToggleObject::TextToggleObject()
{
	m_toggle = false;
}

void TextToggleObject::Update()
{
	if (GET_SINGLE(Input)->GetButtonDown(m_toggleKey))
	{
		m_toggle = !m_toggle;
	}
}

void TextToggleObject::Render(const ComPtr<ID2D1DeviceContext2>& device)
{
	if (m_toggle)
		TextObject::Render(device);
}

DebugTextObject::DebugTextObject()
{
	m_player = nullptr;
	m_camera = nullptr;

	SetToggleKey(KEY_TYPE::F1);
	SetBrush("WHITE");
	SetFormat("15L");
	SetText(L"");
	SetPivot(ePivot::LEFTTOP);
	SetScreenPivot(ePivot::LEFTTOP);
	SetPosition({ 0.0f, 0.0f });
}

void DebugTextObject::Update()
{
	TextToggleObject::Update();
	m_player = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainPlayerScript();
	m_camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	m_light = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainLight();
	

	wstring debugText{};
	if (!m_player) {
		debugText += L"Player Position : NULL\n";
		debugText += L"Player Velocity : NULL\n";
		debugText += L"Player Rotation : NULL\n";
	}
	else {
		debugText += L"Player Position : " + to_wstring((int)m_player->GetRigidBody()->GetPosition().x) + L", "
			+ to_wstring((int)m_player->GetRigidBody()->GetPosition().y) + L", " + to_wstring((int)m_player->GetRigidBody()->GetPosition().z) + L"\n";

		debugText += L"Player Velocity : " + to_wstring((int)m_player->GetRigidBody()->GetLinearVelocity().x) + L", "
			+ to_wstring((int)m_player->GetRigidBody()->GetLinearVelocity().y) + L", " + to_wstring((int)m_player->GetRigidBody()->GetLinearVelocity().z) + L"\n";

		debugText += L"Player Rotation : " + to_wstring((int)XMConvertToDegrees(m_player->GetTransform()->GetLocalRotation().x)) + L", "
			+ to_wstring((int)XMConvertToDegrees(m_player->GetTransform()->GetLocalRotation().y)) + L", " + to_wstring((int)XMConvertToDegrees(m_player->GetTransform()->GetLocalRotation().z)) + L"\n";

	}

	debugText += L"\n";
	
	if (!m_camera) {
		debugText += L"MainCamera : NULL\n";
	}
	else {
		debugText += L"MainCamera m_vecDeffered Size : " + to_wstring(m_camera->GetVecDeferred().size()) + L"\n";
		debugText += L"MainCamera m_vecForward Size : " + to_wstring(m_camera->GetVecForward().size()) + L"\n";
		debugText += L"DirectionalLight m_vecShadow Size : " + to_wstring(m_light->GetShadowCamera()->GetCamera()->GetVechadow().size()) + L"\n";
	}

	SetText(debugText);
}

HPTextObject::HPTextObject()
{
	m_displayHP = 0.0f;
	m_scale = 1.0f;
	m_timerOn = false;
	m_timer = 0.0f;
	m_rect = { 0.0f, 0.0f, 100.0f, 0.0f };
}

void HPTextObject::Render(const ComPtr<ID2D1DeviceContext2>& device)
{
	auto player = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainPlayerScript();
	wstring hpText{ to_wstring(static_cast<int>(m_displayHP) <= 0 ? 0 : static_cast<int>(m_displayHP)) };
	m_text = hpText;
	SetFormat("30L");
	CalcWidthHeight();
	float hpWidth = m_width;

	D2D1::Matrix3x2F matrix{};
	matrix.SetProduct(D2D1::Matrix3x2F::Scale(m_scale, m_scale, {hpWidth, m_rect.bottom}), D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));
	device->SetTransform(matrix);
	device->DrawText(hpText.c_str(), static_cast<UINT32>(hpText.size()), s_formats["30L"].Get(), &m_rect, s_brushes["WHITE"].Get());

	wstring slash{ TEXT("/") };
	device->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x + hpWidth, m_position.y));
	device->DrawText(slash.c_str(), static_cast<UINT32>(slash.size()), s_formats["24L"].Get(), &m_rect, s_brushes["WHITE"].Get());

	m_text = slash;
	m_format = "24L";
	CalcWidthHeight();
	float slashWidth{ m_width };

	wstring maxHpText{ to_wstring(static_cast<int>(player->GetMaxHP()))};
	device->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x + hpWidth + slashWidth, m_position.y));
	device->DrawText(maxHpText.c_str(), static_cast<UINT32>(maxHpText.size()), s_formats["24L"].Get(), &m_rect, s_brushes["WHITE"].Get());
}

void HPTextObject::Update()
{
	auto player = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainPlayerScript();
	if (abs(m_displayHP - player->GetHP()) > 0.0001f)
	{
		m_displayHP = player->GetHP();
		m_timer = 0.0f;
		m_timerOn = true;
	}

	m_text = to_wstring(static_cast<int>(player->GetHP()));
	m_format = "30R";
	CalcWidthHeight();
	float hpWidth = m_width;
	m_rect.bottom = m_height;

	float angle{ 30.0f + 150.0f * (m_timer / 0.2f) };
	m_scale = 1.0f + sinf(XMConvertToRadians(angle)) * 0.2f;

	if (m_timerOn) m_timer += DELTA_TIME;
	if (m_timer > 0.2f) m_timerOn = false;
}