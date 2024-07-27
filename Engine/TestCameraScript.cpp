#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Terrain.h"
#include "Network.h"
#include "Packet.h"

TestCameraScript::TestCameraScript()
{
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{

	Vec3 pos = GetTransform()->GetLocalPosition();

	if(INPUT->GetButton(KEY_TYPE::P))
		PostQuitMessage(0);

	if(INPUT->GetButton(KEY_TYPE::UP))
		pos += GetTransform()->GetLook() * m_speed * DELTA_TIME;

	if(INPUT->GetButton(KEY_TYPE::DOWN))
		pos -= GetTransform()->GetLook() * m_speed * DELTA_TIME;

	if(INPUT->GetButton(KEY_TYPE::LEFT))
		pos -= GetTransform()->GetRight() * m_speed * DELTA_TIME;

	if(INPUT->GetButton(KEY_TYPE::RIGHT))
		pos += GetTransform()->GetRight() * m_speed * DELTA_TIME;

	if(INPUT->GetButton(KEY_TYPE::F5))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 2.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButton(KEY_TYPE::F6))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 1.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButton(KEY_TYPE::F7))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DELTA_TIME * 1.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButton(KEY_TYPE::F8))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 1.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}
	GetTransform()->SetLocalPosition(pos);

}