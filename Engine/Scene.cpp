#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "MeshRenderer.h"
#include "OcNode.h"
#include "OcTree.h"
#include "Network.h"

Scene::Scene()
{
	
}

void Scene::Awake()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}

}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->FinalUpdate();
	}
	m_ocTree->Update();
}

shared_ptr<Camera> Scene::GetMainCamera()
{
	if (m_cameras.empty())
		return nullptr;

	return m_cameras[0];
}

void Scene::Render()
{
	PushLightData();

	ClearRTV();

	RenderShadow();

	RenderDeferred();

	RenderLights();

	RenderFinal();

	RenderForward();
}

void Scene::ClearRTV()
{
	// SwapChain Group �ʱ�ȭ
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	// Shadow Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	// Deferred Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	// Lighting Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : m_lights)
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	// Deferred OMSet
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// ������ �׸���.
	for (auto& light : m_lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->Render_Forward();

	for (auto& camera : m_cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::RenderUI()
{

}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : m_lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetRigidBody() != nullptr)
	{
		m_collidableGameObjects.push_back(gameObject);
		m_ocTree->InsertObjectCollider(gameObject->GetCollider());
	}


	if (gameObject->GetCamera() != nullptr)
	{
		m_cameras.push_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		m_lights.push_back(gameObject->GetLight());
	}

	m_gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetRigidBody())
	{
		auto findIt = std::find(m_collidableGameObjects.begin(), m_collidableGameObjects.end(), gameObject);
		if (findIt != m_collidableGameObjects.end())
			m_collidableGameObjects.erase(findIt);
	}

	if (gameObject->GetCamera())
	{
		auto findIt = std::find(m_cameras.begin(), m_cameras.end(), gameObject->GetCamera());
		if (findIt != m_cameras.end())
			m_cameras.erase(findIt);
	}
	else if (gameObject->GetLight())
	{
		auto findIt = std::find(m_lights.begin(), m_lights.end(), gameObject->GetLight());
		if (findIt != m_lights.end())
			m_lights.erase(findIt);
	}

	auto findIt = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
	if (findIt != m_gameObjects.end())
		m_gameObjects.erase(findIt);

}


void Scene::IntersectColliders(shared_ptr<BaseCollider> collider1, shared_ptr<BaseCollider> collider2)
{
	if (!collider1 || !collider2)
		return;

	switch (collider2->GetColliderType()) {
	case ColliderType::Sphere: {
		auto sphereCollider = dynamic_pointer_cast<SphereCollider>(collider2);

		if (collider1->Intersects(sphereCollider->GetBoundingSphere())) {
			collider1->setColor(Vec4(1, 0, 0, 0), true);
			collider2->setColor(Vec4(1, 0, 0, 0), true);
		}
		break;
	}
	case ColliderType::Box: {
		auto boxCollider = dynamic_pointer_cast<BoxCollider>(collider2);
		if (collider1->Intersects(boxCollider->GetBoundingBox())) {
			collider1->setColor(Vec4(1, 0, 0, 0), true);
			collider2->setColor(Vec4(1, 0, 0, 0), true);
		}
		break;
	}
	default:
		// Handle other collider types if necessary
		break;
	}
}

void Scene::testCollision()
{
	for (auto& cgo : m_collidableGameObjects)
	{
		cgo->GetCollider()->setColor(Vec4(0,0,0,0),false);
	}
	for (auto& cgo : m_collidableGameObjects)
	{
		m_ocTree->CollisionInspection(cgo->GetCollider());
	}

	/*for (int i = 0; i < m_collidableGameObjects.size(); ++i) {
		for (int j = i + 1; j < m_collidableGameObjects.size(); ++j) {
			IntersectColliders(m_collidableGameObjects[i]->GetCollider(), m_collidableGameObjects[j]->GetCollider());
		}
	}*/
}
