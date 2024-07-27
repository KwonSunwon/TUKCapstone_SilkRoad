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
#include "Mainstage2.h"
#include "MainStage3.h"
#include "LobbyStage.h"
#include "Network.h"

#include "UIObject.h"
#include "TextObject.h"
#include "CanvasObject.h"

#include "DifficultyManager.h"

void SceneManager::Update()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->Update();
	m_activeScene->LateUpdate();
	m_activeScene->FinalUpdate();
	m_activeScene->GetMainCamera()->GetTransform()->FinalUpdate();
	m_activeScene->GetMainCamera()->FinalUpdate();

	for (int i = 0; i < m_iterations; ++i) {
		m_activeScene->PhysicsStep(m_iterations);
		m_activeScene->testCollision();
	}

	if (reset)
	{
		switch (s)
		{
		case 0:
			m_activeScene = LoadLobbyScene();
			m_activeScene->Awake();
			m_activeScene->Start();
			break;
		case 1:
			m_activeScene = LoadMainScene1();
			m_activeScene->Awake();
			m_activeScene->Start();
			break;
		case 2:
			m_activeScene = LoadMainScene2();
			m_activeScene->Awake();
			m_activeScene->Start();
			break;
		case 3:
			m_activeScene = LoadMainScene3();
			m_activeScene->Awake();
			m_activeScene->Start();
			break;
		}

		if (s == 1)
			GET_SINGLE(DifficultyManager)->Init();

		reset = false;
	}
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
	D2D1_SIZE_F rtSize = device->GetD3D11On12RT(backbufferindex)->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);

	// Acquire our wrapped render target resource for the current back buffer.
	device->GetD3D11on12Device()->AcquireWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);
	// Render text directly to the back buffer.
	device->GetD2DDeviceContext()->SetTarget(device->GetD3D11On12RT(backbufferindex).Get());
	device->GetD2DDeviceContext()->BeginDraw();

	if (m_activeScene)
		m_activeScene->RenderUI();


	// 호스트 방 개설 시 IP 출력
	std::wostringstream ss3;
	ss3 << "";
	static float connectInfoTime = 3.f;
	if (GET_SINGLE(NetworkManager)->m_displayVar == 1)
	{
		GET_SINGLE(NetworkManager)->GetMyIP();
		ss3 << L"Host IP : " << GET_SINGLE(NetworkManager)->GetMyIP();
	}
	else if (GET_SINGLE(NetworkManager)->m_displayVar == 2 && connectInfoTime > 0.f)
	{
		ss3 << L"Guest1 Connected";
		connectInfoTime -= DELTA_TIME;
	}
	std::wstring hostIPDrawText;
	hostIPDrawText = ss3.str();

	textRect = D2D1::RectF(0, 800, 300, 900);

	device->GetD2DDeviceContext()->DrawText(
		hostIPDrawText.c_str(),
		static_cast<UINT32>(hostIPDrawText.length()),
		device->GetTextFormat().Get(),
		&textRect,
		device->GetSolidColorBrush().Get()
	);

	// 보스 사망 시 게임 클리어 메시지 출력
	std::wostringstream ss4;
	ss4 << "";
	if (GET_SINGLE(SceneManager)->GetActiveScene()->m_bossMonsterScript != nullptr)
	{
		if (GET_SINGLE(SceneManager)->GetActiveScene()->m_bossMonsterScript->GetRigidBody()->GetStatic())
		{
			ss4 << L"Stage1 Clear!";
		}
	}
	std::wstring stageClearDrawText;
	stageClearDrawText = ss4.str();
	
	textRect = D2D1::RectF(600, 100, 1000, 200);
	device->GetD2DDeviceContext()->DrawText(
		stageClearDrawText.c_str(),
		static_cast<UINT32>(stageClearDrawText.length()),
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
	// TODO : ���� Scene ����
	// TODO : ���Ͽ��� Scene ���� �ε�

	m_activeScene = LoadL();

	m_activeScene->Awake();
	m_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// ���� ������ ����
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

	// ViewSpace���� Picking ����
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

		// ViewSpace������ Ray ����
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace������ Ray ����
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace���� ����
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

void SceneManager::LoadTextBrush()
{
	auto device = GEngine->GetD3D11On12Device()->GetD2DDeviceContext();
	device->CreateSolidColorBrush(D2D1::ColorF{ 0x1e1e1e }, &TextObject::s_brushes["BLACK"]);
	device->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Red }, &TextObject::s_brushes["RED"]);
	device->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::DeepSkyBlue }, &TextObject::s_brushes["BLUE"]);
	device->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &TextObject::s_brushes["WHITE"]);
	device->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Yellow }, &TextObject::s_brushes["YELLOW"]);
}

void SceneManager::LoadTextFormats()
{
	auto dwFacory = GEngine->GetD3D11On12Device()->GetDWriteFactory();

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		30,
		TEXT("ko-kr"),
		&TextObject::s_formats["default"]
	);
	TextObject::s_formats["default"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	TextObject::s_formats["default"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12,
		TEXT("ko-kr"),
		&TextObject::s_formats["12L"]
	);
	TextObject::s_formats["12L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["12L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12,
		TEXT("ko-kr"),
		&TextObject::s_formats["12R"]
	);
	TextObject::s_formats["12R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["12R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15,
		TEXT("ko-kr"),
		&TextObject::s_formats["15L"]
	);
	TextObject::s_formats["15L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["15L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15,
		TEXT("ko-kr"),
		&TextObject::s_formats["15R"]
	);
	TextObject::s_formats["15R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["15R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15,
		TEXT("ko-kr"),
		&TextObject::s_formats["15C"]
	);
	TextObject::s_formats["15C"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	TextObject::s_formats["15C"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18,
		TEXT("ko-kr"),
		&TextObject::s_formats["18L"]
	);
	TextObject::s_formats["18L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["18L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18,
		TEXT("ko-kr"),
		&TextObject::s_formats["18R"]
	);
	TextObject::s_formats["18R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["18R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24,
		TEXT("ko-kr"),
		&TextObject::s_formats["24L"]
	);
	TextObject::s_formats["24L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["24L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24,
		TEXT("ko-kr"),
		&TextObject::s_formats["24R"]
	);
	TextObject::s_formats["24R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["24R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24,
		TEXT("ko-kr"),
		&TextObject::s_formats["24C"]
	);
	TextObject::s_formats["24C"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	TextObject::s_formats["24C"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		30,
		TEXT("ko-kr"),
		&TextObject::s_formats["30L"]
	);
	TextObject::s_formats["30L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["30L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		30,
		TEXT("ko-kr"),
		&TextObject::s_formats["30R"]
	);
	TextObject::s_formats["30R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["30R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32,
		TEXT("ko-kr"),
		&TextObject::s_formats["32L"]
	);
	TextObject::s_formats["32L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["32L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32,
		TEXT("ko-kr"),
		&TextObject::s_formats["32R"]
	);
	TextObject::s_formats["32R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["32R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		34,
		TEXT("ko-kr"),
		&TextObject::s_formats["34L"]
	);
	TextObject::s_formats["34L"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	TextObject::s_formats["34L"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		34,
		TEXT("ko-kr"),
		&TextObject::s_formats["34R"]
	);
	TextObject::s_formats["34R"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	TextObject::s_formats["34R"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	dwFacory->CreateTextFormat(
		TEXT("나눔스퀘어OTF"),
		NULL,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		34,
		TEXT("ko-kr"),
		&TextObject::s_formats["34C"]
	);
	TextObject::s_formats["34C"]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	TextObject::s_formats["34C"]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);




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

		// UAV �� Texture ����
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ������ �׷� (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(100000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 900.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
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
		camera->GetCamera()->SetCullingMaskAll(); // �� ����
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI�� ����
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region TextUIResources
	{
		LoadTextBrush();
		LoadTextFormats();
	}
#pragma endregion

#pragma region TextUI
	{
		auto loadingText = make_shared<TextObject>();
		loadingText->SetFormat("default");
		loadingText->SetBrush("WHITE");
		loadingText->SetText(TEXT("5번 키를 눌러 게임 시작"));
		loadingText->SetPivot(ePivot::CENTER);
		loadingText->SetScreenPivot(ePivot::CENTER);
		loadingText->SetPosition(Vec2(0.f, 0.f));
		scene->AddTextObject(loadingText);
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



	return scene;

}
