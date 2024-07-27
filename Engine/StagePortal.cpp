#include "pch.h"
#include "StagePortal.h"
#include "TextObject.h"
#include "UIObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"

void StagePortal::Update()
{
	if (m_isActivated)
	{
		if (m_timer > 0.f)
		{
			m_timer -= GET_SINGLE(Timer)->GetDeltaTime();
		}
	}

}

void StagePortal::PrintInteractiveText()
{
	wstring text{};
	
	if (m_isActivated)
	{
		if (m_timer > 0.f)
			text = (L"게이트 활성화 중 ...");
		else
			text = (L"다음 스테이지로 이동");
	}
	else
		text = (L"게이트 활성화");


	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetText(text);
	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetPosition(Vec2(0.f, -30.f));
	GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetVisible(true);
}

void StagePortal::InteractiveFunction()
{
	if (m_isActivated == false)
	{
		m_isActivated = true;
		m_timer = 60.f;
	}
	else
	{
		if(m_timer <= 0)
			GET_SINGLE(SceneManager)->StartNextStage();
	}
}
