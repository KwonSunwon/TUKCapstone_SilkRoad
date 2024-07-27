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

	if(INPUT->GetButton(KEY_TYPE::Q))
	{
		/*Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 2.5f;
		GetTransform()->SetLocalRotation(rotation);*/
	}

	if(INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 2.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButton(KEY_TYPE::Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DELTA_TIME * 2.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 2.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if(INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}
	GetTransform()->SetLocalPosition(pos);

	Vec3 rot = GetTransform()->GetLocalRotation();
	const POINT& mouseDelta = INPUT->GetMouseDelta();

	// Rotate according to mouse movement


	if(INPUT->GetButton(KEY_TYPE::KEY_4))
	{
		GET_SINGLE(SceneManager)->ResetStage();
	}
	if(INPUT->GetButton(KEY_TYPE::KEY_5))
	{
		GET_SINGLE(SceneManager)->StartNextStage();
		if(GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST) {
			shared_ptr<StageChangePacket> packet = make_shared<StageChangePacket>();
			packet->m_stageIndex = GET_SINGLE(SceneManager)->GetStageIndex();
			SEND(packet);
		}

	}

	// 관전 전환
	if(INPUT->GetButtonDown(KEY_TYPE::C)) {
		m_playerType = (m_playerType + 1) % 3;
		GET_SINGLE(SceneManager)->GetActiveScene()->ChangeSpectate(static_cast<PlayerType>(m_playerType));
	}

	/*if (rot.x + mouseDelta.y * 0.001f < XMConvertToRadians(40.f) && rot.x + mouseDelta.y * 0.001f > XMConvertToRadians(-40.f))
		rot.x += mouseDelta.y * 0.001f;

	GetTransform()->SetLocalRotation(rot);*/

}