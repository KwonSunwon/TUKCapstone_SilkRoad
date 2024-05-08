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
#include "MapObjectsLoader.h"

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OrientedBoxCollider.h"
#include "MeshData.h"
#include "TestDragon.h"
#include "Timer.h"
#include "TestPlayer.h"
#include "Player.h"
#include "PlayerAnimation.h"
#include "Bomb.h"
#include "Item.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "MainStage1.h"
#include "Network.h"
#include "NetworkPlayer.h"

void SceneManager::Update()
{
	if(m_activeScene == nullptr)
		return;

	m_activeScene->Update();
	m_activeScene->LateUpdate();
	m_activeScene->FinalUpdate();
	m_activeScene->GetMainCamera()->GetTransform()->FinalUpdate();
	m_activeScene->GetMainCamera()->FinalUpdate();
	
	for(int i = 0; i < m_iterations; ++i) {
		m_activeScene->PhysicsStep(m_iterations);
		m_activeScene->testCollision();
	}
	if (reset) {
		reset = false;
		m_activeScene = LoadMainScene();
		s = 1;
		m_activeScene->Awake();
		m_activeScene->Start();
	}

}

// TEMP
void SceneManager::Render()
{
	if(m_activeScene)
		m_activeScene->Render();
}
void SceneManager::RenderUI(shared_ptr<D3D11On12Device> device)
{
	uint8 backbufferindex = GEngine->GetSwapChain()->GetBackBufferIndex();
	if(m_activeScene)
		m_activeScene->RenderUI();
	D2D1_SIZE_F rtSize = device->GetD3D11On12RT(backbufferindex)->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
	
	shared_ptr<GameObject> player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayers()[0];
	//Vec3 playerPos = player->GetTransform()->GetLocalPosition();
	static const WCHAR text[] = L"";
	//static const WCHAR text[] = L"11On12";

	// 디버깅용 플레이어 좌표 텍스트 변환
	std::wostringstream ss;
	//ss << L"X:" << playerPos.x << L", Y:" << playerPos.y << L", Z:" << playerPos.z;
	if (s == 0)
		ss << "Loading";
	else
		ss << "+";
	std::wstring playerPosText = ss.str();

	// Acquire our wrapped render target resource for the current back buffer.
	device->GetD3D11on12Device()->AcquireWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);

	// Render text directly to the back buffer.
	device->GetD2DDeviceContext()->SetTarget(device->GetD3D11On12RT(backbufferindex).Get());
	device->GetD2DDeviceContext()->BeginDraw();
	device->GetD2DDeviceContext()->SetTransform(D2D1::Matrix3x2F::Identity());
	device->GetD2DDeviceContext()->DrawText(
		playerPosText.c_str(),
		static_cast<UINT32>(playerPosText.length()),
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
	// TODO : ??????? Scene ???? ?��?

	m_activeScene = LoadL();

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
	if(findIt == m_layerIndex.end())
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

	for(auto& gameObject : gameObjects)
	{
		if(gameObject->GetCollider() == nullptr)
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
		if(gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if(distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}


shared_ptr<Scene> SceneManager::LoadL()
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
		camera->GetCamera()->SetFar(100000.f);
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
		//scene->AddGameObject(obj);
	}

	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(1920, 1200.f, 1000.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Loading", L"..\\Resources\\Texture\\Loading.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);

			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}

#pragma endregion

#pragma region First Network Characters Setting Example
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Characters.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[idx];
		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(Vec3(3500.f, 1500.f, 2500.f));
			//transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			//transform->SetLocalRotation(Vec3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetMass(80.f);
			rb->SetRestitution(0.f);
			go->SetCheckFrustum(false);
			go->AddComponent(rb);
		}

		//콜라이더 설정 
		//콜라이더의 위치,회전은 Gameobject의 Transform을 사용
		{
			//OBB를 사용할 경우 이곳의 주석을 풀어서 사용
			shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
			collider->SetExtent(Vec3(50, 100, 50));

			//Sphere를 사용할경우 이곳의 주석을 풀어서 사용
			/*shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			collider->SetRadius(100.f);*/



			collider->SetOffset(Vec3(0, 80, 0));
			go->AddComponent(collider);
		}

		//디버그용 콜라이더 매쉬 설정
		if (DEBUG_MODE)
		{
			scene->AddGameObject(go->GetCollider()->GetDebugCollider());
		}

		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		//추가적인 컴포넌트 부착
		{
			shared_ptr<NetworkPlayer> networkPlayer = make_shared<NetworkPlayer>();
			go->AddComponent(networkPlayer);
			scene->m_networkPlayers[0] = networkPlayer;
			//go->AddComponent(make_shared<PlayerAnimation>());
		}


		scene->SetPlayer(go, GUEST_PLAYER1);
		
		scene->AddGameObject(go);

	}



#pragma endregion

#pragma region Second Network Characters Setting Example
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Characters.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[idx];
		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(Vec3(4500.f, 1500.f, 2500.f));
			//transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			//transform->SetLocalRotation(Vec3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetMass(80.f);
			rb->SetRestitution(0.f);
			go->SetCheckFrustum(false);
			go->AddComponent(rb);
		}

		//콜라이더 설정 
		//콜라이더의 위치,회전은 Gameobject의 Transform을 사용
		{
			//OBB를 사용할 경우 이곳의 주석을 풀어서 사용
			shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
			collider->SetExtent(Vec3(50, 100, 50));

			//Sphere를 사용할경우 이곳의 주석을 풀어서 사용
			/*shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			collider->SetRadius(100.f);*/



			collider->SetOffset(Vec3(0, 80, 0));
			go->AddComponent(collider);
		}

		//디버그용 콜라이더 매쉬 설정
		if (DEBUG_MODE)
		{
			scene->AddGameObject(go->GetCollider()->GetDebugCollider());
		}

		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		//추가적인 컴포넌트 부착
		{
			shared_ptr<NetworkPlayer> networkPlayer = make_shared<NetworkPlayer>();
			go->AddComponent(networkPlayer);
			scene->m_networkPlayers[1] = networkPlayer;
			//go->AddComponent(make_shared<PlayerAnimation>());
		}


		scene->SetPlayer(go, GUEST_PLAYER2);

		scene->AddGameObject(go);

	}



#pragma endregion

#pragma region Enemy
	{
		for (int i = 0; i < 25; ++i)
		{
			int idx = 0;
			shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\BR_Characters.fbx");
			vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
			shared_ptr<GameObject> go = gameObjects[idx];
			//Transform 설정
			{
				shared_ptr<Transform> transform = go->GetTransform();
				transform->SetLocalPosition(Vec3(5000.f + (i / 5) * 1000.f, 1500.f, 5000.f + i % 5 * 1000.f));
				transform->SetLocalScale(Vec3(1.2f, 1.2f, 1.2f));
				//transform->SetLocalRotation(Vec3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
			}


#pragma region Terrain
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<Terrain>());
		obj->AddComponent(make_shared<MeshRenderer>());

		obj->GetTransform()->SetLocalScale(Vec3(781.25f, 1580.f, 781.25f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		obj->SetStatic(true);
		obj->GetTerrain()->Init(64, 64);
		obj->SetCheckFrustum(false);

		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetMass(1000.f);
			rb->SetRestitution(0.f);
			obj->SetCheckFrustum(false);
			obj->AddComponent(rb);
		}

		//콜라이더 설정 
		//콜라이더의 위치,회전은 Gameobject의 Transform을 사용
		{
			//OBB를 사용할 경우 이곳의 주석을 풀어서 사용
			shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
			collider->SetExtent(Vec3(100, 100, 100));

			//Sphere를 사용할경우 이곳의 주석을 풀어서 사용
			//shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			//collider->SetRadius(100.f);



			//collider->SetOffset(Vec3(0, 80, 0));
			obj->AddComponent(collider);
		}
		//디버그용 콜라이더 매쉬 설정
		if (DEBUG_MODE)
		{
			scene->AddGameObject(obj->GetCollider()->GetDebugCollider());
		}

		scene->AddGameObject(obj);
		scene->m_terrain = obj->GetTerrain();

	}
#pragma endregion



		for (int i = 0; i < 10; ++i) {


			shared_ptr<GameObject> gm = make_shared<GameObject>();
			gm->AddComponent(make_shared<Transform>());
			gm->GetTransform()->SetLocalScale(Vec3(150.f, 100.f, 100.f));
			gm->GetTransform()->SetLocalPosition(Vec3(1500.f+ 50.f*i, 1500.f + 400.f *i, 2000.f + 0*i ));

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadCubeMesh();
				meshRenderer->SetMesh(mesh);
			}

			{
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);

				material->SetInt(3, 1);
				material->SetVec4(3, Vec4(1, 1, 1, 1));
				meshRenderer->SetMaterial(material);
			}
			gm->AddComponent(meshRenderer);
			
			gm->AddComponent(make_shared<RigidBody>());
			//gm->AddComponent(make_shared<TestDragon>());

			if (i & 1) {
				gm->AddComponent(make_shared<OrientedBoxCollider>());
				gm->GetCollider()->SetExtent(Vec3(75, 50, 50));
				

				/*gm->AddComponent(make_shared<SphereCollider>());
				gm->GetCollider()->SetRadius(100.f);*/


			}
			else {
				/*gm->AddComponent(make_shared<SphereCollider>());
				gm->GetCollider()->SetRadius(100.f);*/

				gm->AddComponent(make_shared<OrientedBoxCollider>());
				gm->GetCollider()->SetExtent(Vec3(75, 50, 50));


			}



			if (gm->GetCollider()->GetDebugCollider() != nullptr)
				scene->AddGameObject(gm->GetCollider()->GetDebugCollider());
			scene->AddGameObject(gm);
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
