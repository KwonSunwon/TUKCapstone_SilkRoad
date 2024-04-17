#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "RigidBody.h"

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "MeshData.h"
#include "TestDragon.h"

#include "TestPlayer.h"

#include "Network.h"

void SceneManager::Update()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->Update();
	m_activeScene->LateUpdate();
	//collision????
	m_activeScene->testCollision();


	m_activeScene->FinalUpdate();
}

// TEMP
void SceneManager::Render()
{
	if (m_activeScene)
		m_activeScene->Render();
}
void SceneManager::RenderUI(shared_ptr<D3D11On12Device> device)
{
	uint8 backbufferindex = GEngine->GetSwapChain()->GetBackBufferIndex();
	if (m_activeScene)
		m_activeScene->RenderUI();
	D2D1_SIZE_F rtSize = device->GetD3D11On12RT(backbufferindex)->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
	static const WCHAR text[] = L"11On12";

	// Acquire our wrapped render target resource for the current back buffer.
	device->GetD3D11on12Device()->AcquireWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);

	// Render text directly to the back buffer.
	device->GetD2DDeviceContext()->SetTarget(device->GetD3D11On12RT(backbufferindex).Get());
	device->GetD2DDeviceContext()->BeginDraw();
	device->GetD2DDeviceContext()->SetTransform(D2D1::Matrix3x2F::Identity());
	device->GetD2DDeviceContext()->DrawText(
		text,
		_countof(text) - 1,
		device->GetTextFormat().Get(),
		&textRect,
		device->GetSolidColorBrush().Get()
	);
	device->GetD2DDeviceContext()->EndDraw();

	// Release our wrapped render target resource. Releasing 
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	device->GetD3D11on12Device()->ReleaseWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);

	// Flush to submit the 11 command list to the shared command queue.
	device->GetD3D11DeviceContext()->Flush();
}

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : ???? Scene ????
	// TODO : ??????? Scene ???? ?ех?

	m_activeScene = LoadTestScene();

	m_activeScene->Awake();
	m_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// ???? ?????? ????
	const wstring& prevName = m_layerNames[index];
	m_layerIndex.erase(prevName);

	m_layerNames[index] = name;
	m_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = m_layerIndex.find(name);
	if (findIt == m_layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace???? Picking ????
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace?????? Ray ????
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace?????? Ray ????
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace???? ????
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}
shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV ?? Texture ????
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ?????? ??? (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45??
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 900.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI?? ?? ????
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // ?? ????
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI?? ????
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\Sky01.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

#pragma region Object
	{
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->SetName(L"OBJ");
			obj->AddComponent(make_shared<Transform>());
			obj->AddComponent(make_shared<SphereCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			obj->GetTransform()->SetLocalPosition(Vec3(0, 900.f, 300.f));
			obj->SetStatic(false);
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
				meshRenderer->SetMesh(cubeMesh);
			}
			{
				shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
				meshRenderer->SetMaterial(material->Clone());
			}

			obj->AddComponent(make_shared<TestPlayer>(0));

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
			scene->SetPlayer(obj, static_cast<PlayerType>(0));
		}
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->SetName(L"OBJ");
			obj->AddComponent(make_shared<Transform>());
			obj->AddComponent(make_shared<SphereCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			obj->GetTransform()->SetLocalPosition(Vec3(0, 900.f, 300.f));
			obj->SetStatic(false);
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
				meshRenderer->SetMesh(cubeMesh);
			}
			{
				shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
				meshRenderer->SetMaterial(material->Clone());
			}

			obj->AddComponent(make_shared<TestPlayer>(1));

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
			scene->SetPlayer(obj, static_cast<PlayerType>(1));
		}
	}
#pragma endregion

#pragma region Terrain
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<Terrain>());
		obj->AddComponent(make_shared<MeshRenderer>());

		obj->GetTransform()->SetLocalScale(Vec3(500.f, 2500.f, 500.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		obj->SetStatic(true);
		obj->GetTerrain()->Init(64, 64);
		obj->SetCheckFrustum(false);

		scene->AddGameObject(obj);
		scene->m_terrain = obj;
	}
#pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 500, 0));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region FBX
	{
		//shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Chr_ScifiWorlds_AlienArmor_04.fbx");
		////shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\tank_trexhwm.fbx");

		//for (int i = 0; i < 10; ++i) {
		//	for (int j = 0; j < 10; ++j) {
		//		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		//		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		//		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(-150.f + 400.f * i, 500.f, 2000.f + 400.f * j));
		//		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		//		gameObjects[0]->SetCheckFrustum(false);

		//		gameObjects[0]->AddComponent(make_shared<TestDragon>());

		//		gameObjects[0]->AddComponent(make_shared<RigidBody>());
		//		gameObjects[0]->GetRigidBody()->m_useGravity = true;
		//		if (i & 1) {
		//			gameObjects[0]->AddComponent(make_shared<BoxCollider>());
		//			gameObjects[0]->GetCollider()->SetExtent(Vec3(50, 100, 50));
		//			gameObjects[0]->GetCollider()->SetCenter(Vec3(-150.f + 400.f * i, 500.f, 2000.f + 400.f * j));
		//		}
		//		else {
		//			gameObjects[0]->AddComponent(make_shared<SphereCollider>());
		//			gameObjects[0]->GetCollider()->SetRadius(90);
		//			gameObjects[0]->GetCollider()->SetCenter(Vec3(-150.f + 400.f * i, 500.f, 2000.f + 400.f * j));
		//		}
		//		scene->AddGameObject(gameObjects[0]->GetCollider()->m_go);
		//		scene->AddGameObject(gameObjects[0]);
		//	}
		//}
		//{
		//	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		//	gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		//	gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(-100, -200, 300));
		//	gameObjects[0]->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		//	gameObjects[0]->SetCheckFrustum(false);
		//	gameObjects[0]->AddComponent(make_shared<TestDragon>());
		//	gameObjects[0]->AddComponent(make_shared<RigidBody>());
		//	gameObjects[0]->GetRigidBody()->m_useGravity = true;
		//	gameObjects[0]->AddComponent(make_shared<BoxCollider>());
		//	gameObjects[0]->GetCollider()->SetExtent(Vec3(50, 100, 50));
		//	gameObjects[0]->GetCollider()->SetCenter(Vec3(-100, -200, 300));

		//	//gameObjects[0]->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		//	scene->AddGameObject(gameObjects[0]->GetCollider()->m_go);
		//	scene->AddGameObject(gameObjects[0]);
		//}
		//{
		//	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		//	gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		//	gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(-100+500*64, -200, 300));
		//	gameObjects[0]->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		//	gameObjects[0]->SetCheckFrustum(false);

		//	gameObjects[0]->AddComponent(make_shared<RigidBody>());
		//	gameObjects[0]->GetRigidBody()->m_useGravity = true;
		//	gameObjects[0]->AddComponent(make_shared<BoxCollider>());
		//	gameObjects[0]->GetCollider()->SetExtent(Vec3(50, 100, 50));
		//	gameObjects[0]->GetCollider()->SetCenter(Vec3(-100+500*64, -200, 300));

		//	//gameObjects[0]->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		//	scene->AddGameObject(gameObjects[0]->GetCollider()->m_go);
		//	scene->AddGameObject(gameObjects[0]);
		//}	
	}
#pragma endregion

#pragma region Network
	{
		shared_ptr<GameObject> network = make_shared<GameObject>();
		network->SetName(L"Network");
		network->AddComponent(make_shared<NetworkScript>());
		scene->AddGameObject(network);
	}
#pragma endregion

	return scene;
}