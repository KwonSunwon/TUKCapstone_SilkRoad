#include "pch.h"
#include "LobbyStage.h"
#include "Scene.h"
#include "SceneManager.h"
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
#include "DummyTerrain.h"
#include "StagePortal.h"

#include "Network.h"
#include "NetworkPlayer.h"

#include "UIObject.h"
#include "TextObject.h"
#include "CanvasObject.h"

#include "SoundManager.h"
#include "EnemyHP.h"
#include "Input.h"
shared_ptr<class Scene> LoadLobbyScene()
{

	GET_SINGLE(SoundManager)->soundStop(Sounds::BGM_SPACE);
	GET_SINGLE(SoundManager)->soundPlay(Sounds::BGM_SPACE);

#pragma region LayerMask

	GET_SINGLE(SceneManager)->SetLayerName(0, L"Default");
	GET_SINGLE(SceneManager)->SetLayerName(1, L"UI");
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
	scene->SetSceneName(string("Lobby"));

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
		//camera->AddComponent(make_shared<TestCameraScript>());
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

#pragma region MapObjects
	{
		/*std::ifstream jsonFile("..\\Resources\\MapData\\ExportedObjects.json");
		nlohmann::json j;
		jsonFile >> j;
		std::map<std::string, std::string> obj_map;

		for (const auto& obj : j["objects"])
			obj_map[obj["name"].get<std::string>()] = obj["meshName"].get<std::string>();*/

		shared_ptr<MapObjectsLoader> loader = make_shared<MapObjectsLoader>();
		loader->Create(scene);

		loader->Load(L"..\\Resources\\MapData\\ExportedObjectsLobby.json");
	}
#pragma endregion

#pragma InteractiveObjects
	{
		vector<wstring> objectPath = {
			L"..\\Resources\\FBX\\SM_Prop_Vending_Machine_01.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Monitor_01.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Vending_Machine_03.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Vending_Machine_04.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Vending_Machine_05.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Banner_01_A.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Banner_01_B.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Banner_01_C.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Banner_01_D.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Monitor_03.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Monitor_06.fbx",
			L"..\\Resources\\FBX\\SM_Prop_Monitor_07.fbx",
		};
		

		for (int i = 0; i < objectPath.size(); ++i)
		{
			shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(objectPath[i]);
			vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
			shared_ptr<GameObject> gm = gameObjects[0];

			gm->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			gm->GetTransform()->SetLocalPosition(Vec3(0,0,0));


			gm->AddComponent(make_shared<RigidBody>());
			gm->AddComponent(make_shared<OrientedBoxCollider>());

			gm->GetRigidBody()->SetStatic(true);
			gm->GetRigidBody()->SetMass(99999.f);
			gm->GetRigidBody()->SetRestitution(0.f);

			//Instancing 유무 설정(사용:0,0  미사용:0,1)
			gm->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

			if (gm->GetCollider()->GetDebugCollider() != nullptr)
				scene->AddGameObject(gm->GetCollider()->GetDebugCollider());
			gm->SetShadow(true);
			gm->SetCheckFrustum(true);

			Vec3 localPos, localRot, colliderCenter, ColliderSize;

			switch (i)
			{
			case 0:
				// 자판기 1(콜라)
				localPos = Vec3(758.f, 2089.f, -2360.f);
				localRot = Vec3(0.f, 270.f, 0.f);
				colliderCenter = Vec3(-0.23f, 1.88f, -0.03f);
				ColliderSize = Vec3(1.9f, 3.76f, 1.1f);
				break;

			case 1:
				// 자판기 2(모니터)
				localPos = Vec3(758.f, 2089.f, -2609.f);
				localRot = Vec3(0.f, 270.f, 0.f);
				colliderCenter = Vec3(0.f, 0.87f, 0.f);
				ColliderSize = Vec3(0.654f, 2.f, 0.654f);
				break;

			case 2:
				// 자판기 3(원통형)
				localPos = Vec3(744.f, 2089.f, -2876.f);
				localRot = Vec3(0.f, 270.f, 0.f);
				colliderCenter = Vec3(0.f, 1.79f, 0.25f);
				ColliderSize = Vec3(1.7f, 3.6f, 2.1f);
				break;

			case 3:
				// 자판기 4(알약)
				localPos = Vec3(766.f, 2089.f, -3121.f);
				localRot = Vec3(0.f, 270.f, 0.f);
				colliderCenter = Vec3(0.06f, 1.186f, 0.112f);
				ColliderSize = Vec3(1.31f, 2.26f, 1.44f);
				break;

			case 4:
				// 자판기 5(이상한거)
				localPos = Vec3(758.f, 2024.f, -3315.f);
				localRot = Vec3(0.f, 270.f, 0.f);
				colliderCenter = Vec3(-0.16f, 1.37f, 0.116f);
				ColliderSize = Vec3(1.29f, 1.44f, 0.558f);
				break;

			case 5:
				// 클래스 배너 1
				localPos = Vec3(-775.f, 2182.f, -3187.f);
				localRot = Vec3(0.f, 90.f, 0.f);
				colliderCenter = Vec3(0.f, 0.f, 0.f);
				ColliderSize = Vec3(0.952f, 1.9f, 0.05f);
				break;

			case 6:
				// 클래스 배너 2
				localPos = Vec3(-775.f, 2182.f, -2937.5f);
				localRot = Vec3(0.f, 90.f, 0.f);
				colliderCenter = Vec3(0.f, 0.f, 0.f);
				ColliderSize = Vec3(0.952f, 1.9f, 0.05f);
				break;

			case 7:
				// 클래스 배너 3
				localPos = Vec3(-775.f, 2182.f, -2687.5f);
				localRot = Vec3(0.f, 90.f, 0.f);
				colliderCenter = Vec3(0.f, 0.f, 0.f);
				ColliderSize = Vec3(0.952f, 1.9f, 0.05f);
				break;

			case 8:
				// 클래스 배너 4
				localPos = Vec3(-775.f, 2182.f, -2437.5f);
				localRot = Vec3(0.f, 90.f, 0.f);
				colliderCenter = Vec3(0.f, 0.f, 0.f);
				ColliderSize = Vec3(0.952f, 1.9f, 0.05f);
				break;

			case 9:
				// 게임 시작 모니터
				localPos = Vec3(-15.f, 2073.f, -2063.9f);
				localRot = Vec3(0.f, 180.f, 0.f);
				colliderCenter = Vec3(0.f, 0.856f, 0.f);
				ColliderSize = Vec3(1.113f, 1.712f, 0.903f);
				break;

			case 10:
				// 방 개설 모니터
				localPos = Vec3(-15.f -200.f, 2073.f, -2063.9f);
				localRot = Vec3(0.f, 180.f, 0.f);
				colliderCenter = Vec3(0.f, 0.856f, 0.f);
				ColliderSize = Vec3(1.113f, 1.712f, 0.903f);
				break;


			case 11:
				// 방 참가 모니터
				localPos = Vec3(-15.f +200.f, 2073.f, -2063.9f);
				localRot = Vec3(0.f, 180.f, 0.f);
				colliderCenter = Vec3(0.f, 0.856f, 0.f);
				ColliderSize = Vec3(1.113f, 1.712f, 0.903f);
				break;
			}

			localPos.x += 25000.f;
			localPos.z += 25000.f;
			gm->GetTransform()->SetLocalPosition(localPos);

			localRot.y -= 180.f;
			localRot.x = XMConvertToRadians(localRot.x);
			localRot.y = XMConvertToRadians(localRot.y);
			localRot.z = XMConvertToRadians(localRot.z);
			gm->GetTransform()->SetLocalRotation(localRot);

			colliderCenter *= 100.f;
			gm->GetCollider()->SetOffset(colliderCenter);

			ColliderSize *= (100.f / 2.f);
			gm->GetCollider()->SetExtent(ColliderSize);


			Vec3 rot = gm->GetTransform()->GetLocalRotation();
			Matrix rotationMatrix = XMMatrixRotationX(rot.x) * XMMatrixRotationY(rot.y + 3.141592f) * XMMatrixRotationZ(rot.z);
			Vec3 center = XMVector3Transform(gm->GetCollider()->GetOffset(), rotationMatrix);
			gm->GetCollider()->SetOffset(center);

			gm->AddComponent(make_shared<InteractiveObject>(i));

			scene->AddGameObject(gm);
		}
		
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

#pragma region TextUI
	{
		auto loadingText = make_shared<TextObject>();
		loadingText->SetFormat("default");
		loadingText->SetBrush("WHITE");
		loadingText->SetText(L"+");
		loadingText->SetPivot(ePivot::CENTER);
		loadingText->SetScreenPivot(ePivot::CENTER);
		loadingText->SetPosition(Vec2(0.f, 0.f));
		scene->AddTextObject(loadingText);

		auto debugText = make_shared<DebugTextObject>();
		scene->AddTextObject(debugText);

		auto interact = make_shared<InteractiveObjectText>();
		interact->SetFormat("24C");
		interact->SetBrush("WHITE");
		interact->SetText(L"+");
		interact->SetPivot(ePivot::CENTERBOT);
		interact->SetScreenPivot(ePivot::CENTER);
		interact->SetPosition(Vec2(0.f, -50.f));
		scene->AddTextObject(interact);
		scene->SetInteractiveObjectText(interact);

	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(25000.f, 10000, 25000));
		light->AddComponent(make_shared<Light>());
		//light->GetLight()->SetLightDirection(Vec3(0.3f, -1.0f, 1.0f));
		light->GetLight()->SetLightDirection(Vec3(-0.1, -1.0f, 0.2));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		light->AddComponent(make_shared<TestCameraScript>());

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region HUD
	{
		// 캐릭터 스탯
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
			obj->AddComponent(make_shared<Transform>());
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
				meshRenderer->SetMesh(mesh);
			}
			{
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"AlphaTexture");
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"PlayerStat", L"..\\Resources\\Texture\\PlayerStatBase.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<PlayerStatUI> statUI = make_shared<PlayerStatUI>();
			statUI->SetPivot(ePivot::CENTER);
			statUI->SetToggleKey(KEY_TYPE::TAB);
			statUI->SetScreenPivot(ePivot::CENTER);
			statUI->SetWidth(1400.f);
			statUI->SetHeight(700.f);
			statUI->SetPosition(Vec2(0, 0));
			statUI->SetZOrder(300);

			obj->AddComponent(statUI);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);

			// 캐릭터 능력치 텍스트
			{
				auto statText = make_shared<PlayerStatTextObject>();
				statText->SetFormat("18L");
				statText->SetToggleKey(KEY_TYPE::F12);
				statText->SetBrush("WHITE");
				//statText->SetText(L"Player Stat\nMax HP : 100\nHP Regen L 10\nMin Damage : 100\nMax Damage : 120\nWalkSpeed : 100\nJumpPower : 100\nCritical Chance : 50 %\nCritical Damage : 200%\nFire Rate : 2");
				statText->SetPivot(ePivot::LEFTTOP);
				statText->SetScreenPivot(ePivot::CENTER);
				statText->SetPosition(Vec2(-695, 65));
				statUI->SetPlayerStatText(statText);
				scene->AddTextObject(statText);
			}

			// 캐릭터 클래스 정보 텍스트
			{
				auto classText = make_shared<PlayerClassTextObject>();
				classText->SetFormat("24L");
				classText->SetToggleKey(KEY_TYPE::F12);
				classText->SetBrush("WHITE");
				classText->SetPivot(ePivot::LEFTTOP);
				classText->SetScreenPivot(ePivot::CENTER);
				classText->SetPosition(Vec2(-230, 65));
				statUI->SetPlayerClassText(classText);
				scene->AddTextObject(classText);
			}

			// 캐릭터 아웃게임 업그레이드 텍스트

			{
				auto outgameUpgradeText = make_shared<OutgameUpgradeTextObject>();
				outgameUpgradeText->SetFormat("24L");
				outgameUpgradeText->SetToggleKey(KEY_TYPE::F12);
				outgameUpgradeText->SetBrush("WHITE");
				outgameUpgradeText->SetPivot(ePivot::LEFTTOP);
				outgameUpgradeText->SetScreenPivot(ePivot::CENTER);
				outgameUpgradeText->SetPosition(Vec2(237, 65));
				statUI->SetOutgameUpgradeText(outgameUpgradeText);
				scene->AddTextObject(outgameUpgradeText);
			}

			for (int i = 0; i < 17; ++i)
			{
				// 아이템 아이콘
				{
					shared_ptr<GameObject> obj = make_shared<GameObject>();
					obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
					obj->AddComponent(make_shared<Transform>());
					shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
					{
						shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
						meshRenderer->SetMesh(mesh);
					}
					{
						shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"AlphaTexture");
						shared_ptr<Texture> texture = GET_SINGLE(Resources)->LoadItemIconTexture(i);
						shared_ptr<Material> material = make_shared<Material>();
						material->SetShader(shader);
						material->SetTexture(0, texture);

						meshRenderer->SetMaterial(material);
					}
					shared_ptr<UIToggleObject> uiObject = make_shared<UIToggleObject>();
					uiObject->SetPivot(ePivot::LEFTTOP);
					uiObject->SetToggleKey(KEY_TYPE::F12);
					uiObject->SetScreenPivot(ePivot::CENTER);
					uiObject->SetWidth(80.f);
					uiObject->SetHeight(80.f);
					uiObject->SetPosition(Vec2(-695 + (280 * (i % 5)), 340 - (i / 5) * 100));
					uiObject->SetZOrder(200);

					statUI->AddItemSlot(uiObject, i);
					obj->AddComponent(uiObject);
					obj->AddComponent(meshRenderer);
					scene->AddGameObject(obj);
				}

				// 아이템 텍스트
				{
					auto ItemText = make_shared<TextToggleObject>();
					ItemText->SetFormat("15L");
					ItemText->SetToggleKey(KEY_TYPE::F12);
					ItemText->SetBrush("WHITE");
					ItemText->SetText(L"Brilliant Behemoth\nAdds explosion to bullets.\n\n2");
					ItemText->SetPivot(ePivot::LEFTTOP);
					ItemText->SetScreenPivot(ePivot::CENTER);
					ItemText->SetPosition(Vec2(-610 + (280 * (i % 5)), -340 + (i / 5) * 100));
					statUI->AddItemDesc(ItemText, i);
					scene->AddTextObject(ItemText);
				}
			}

		}

	}
#pragma endregion

#pragma region Characters Setting Example
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Launcher.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[idx];
		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(Vec3(25000.0f, 3000.f, 22000.f));
			//transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			//transform->SetLocalRotation(Vec3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(false);
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
			//go->AddComponent(make_shared<Player>());
			//go->AddComponent(make_shared<PlayerAnimation>());
		}

		//카메라 세팅
		{
			shared_ptr<Camera> camera = scene->GetMainCamera();
			camera->GetTransform()->SetParent(go->GetTransform());
			camera->GetTransform()->SetLocalPosition(Vec3(0.f, 185.f, 40.f));
			camera->GetTransform()->SetLocalRotation(Vec3(XMConvertToRadians(10.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//플레이어 컴포넌트 부착
		{
			shared_ptr<Player> playerScript = make_shared<Player>();
			// 총알 오브젝트 풀 생성
			//for(int i = 0; i < 20; ++i)
			//{
			//	shared_ptr<GameObject> bullet = make_shared<GameObject>();
			//	bullet->SetName(L"Bullet");

			//	{
			//		bullet->AddComponent(make_shared<Transform>());
			//		bullet->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			//		bullet->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
			//	}

			//	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			//	{
			//		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadSphereMesh();
			//		meshRenderer->SetMesh(mesh);
			//	}

			//	{
			//		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
			//		shared_ptr<Material> material = make_shared<Material>();
			//		material->SetShader(shader);

			//		material->SetInt(3, 1);
			//		material->SetVec4(3, Vec4(1, 1, 1, 1));
			//		meshRenderer->SetMaterial(material);
			//	}
			//	bullet->AddComponent(meshRenderer);

			//	{
			//		shared_ptr<RigidBody> rb = make_shared<RigidBody>();
			//		rb->SetStatic(true);
			//		rb->SetRestitution(0.f);
			//		bullet->SetCheckFrustum(false);
			//		bullet->AddComponent(rb);
			//	}
			//	{
			//		//shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			//		//collider->SetRadius(10.f);
			//		shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
			//		collider->SetExtent(Vec3(10, 10, 10));
			//		bullet->AddComponent(collider);
			//	}
			//	{
			//		//bullet->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			//	}
			//	bullet->SetActive(true);

			//	shared_ptr<PlayerBullet> bulletScript = make_shared<PlayerBullet>();
			//	playerScript->AddBullet(bulletScript);

			//	bullet->AddComponent(bulletScript);

			//	shared_ptr<GameObject> bomb = GET_SINGLE(Resources)->LoadBombPrefab(Vec3(0, 0, 0));
			//	bomb->GetRigidBody()->SetOverlap();
			//	shared_ptr<Bomb> bombScript = dynamic_pointer_cast<Bomb>(bomb->GetMonobehaviour("Bomb"));
			//	bulletScript->SetBomb(bombScript);
			//	scene->AddGameObject(bullet);
			//	scene->AddGameObject(bomb);

			//}
			playerScript->SetPlayerCamera(scene->GetMainCamera());
			scene->SetMainPlayerScript(playerScript);
			go->SetShadow(true);
			go->AddComponent(playerScript);
		}

		scene->SetPlayer(go, MAIN_PLAYER);
		scene->AddGameObject(go);

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
			go->SetCheckFrustum(true);
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
			go->SetShadow(true);
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
			transform->SetLocalPosition(Vec3(-4500.f, 1500.f, 2500.f));
			//transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			//transform->SetLocalRotation(Vec3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetMass(80.f);
			rb->SetRestitution(0.f);
			go->SetCheckFrustum(true);
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
			go->SetShadow(true);
		}


		scene->SetPlayer(go, GUEST_PLAYER2);

		scene->AddGameObject(go);

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
