#include "pch.h"
#include "MainStage2.h"
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
#include "TankerSkill.h"
#include "HealerSkill.h"
#include "UpgradeManager.h"
#include "InteractiveObject.h"
#include "StagePortal.h"

#include "Input.h"
shared_ptr<class Scene> LoadMainScene2()
{
	GET_SINGLE(SoundManager)->soundStop(Sounds::BGM_SPACE);
	GET_SINGLE(SoundManager)->soundStop(Sounds::BGM_DESERT);
	GET_SINGLE(SoundManager)->soundStop(Sounds::BGM_SNOWFIELD);
	GET_SINGLE(SoundManager)->soundStop(Sounds::BGM_CORRUPTION);
	GET_SINGLE(SoundManager)->soundPlay(Sounds::BGM_SNOWFIELD);
	GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_WARP);

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

		loader->Load(L"..\\Resources\\MapData\\ExportedObjects2.json");
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
		obj->SetShadow(true);
		obj->GetTerrain()->Init(64, 64, 2);
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

		shared_ptr<DummyTerrain> DT = make_shared<DummyTerrain>();
		DT->SetMonovihaviourName("Terrain");
		obj->AddComponent(DT);
		scene->AddGameObject(obj);
		scene->m_terrain = obj->GetTerrain();

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
		{
			shared_ptr<UIToggleObject> uiObject = make_shared<UIToggleObject>();
			uiObject->SetPivot(ePivot::CENTER);
			uiObject->SetScreenPivot(ePivot::CENTER);
			uiObject->SetWidth(100.f);
			uiObject->SetHeight(100.f);
			uiObject->SetZOrder(100);
			uiObject->SetPosition(Vec2(-350.0f + (i * 120), 250));
			uiObject->SetToggleKey(KEY_TYPE::F1);
			obj->AddComponent(uiObject);
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
		interact->SetFormat("24L");
		interact->SetBrush("WHITE");
		interact->SetText(L"+");
		interact->SetPivot(ePivot::CENTERBOT);
		interact->SetScreenPivot(ePivot::CENTER);
		interact->SetPosition(Vec2(0.f, -50.f));
		scene->AddTextObject(interact);
		scene->SetInteractiveObjectText(interact);

	}
#pragma endregion

#pragma region HUD
	{
		// HP바 베이스
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"PlayerHPBarBase", L"..\\Resources\\Texture\\PlayerHPBarBase.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<UIObject> uiObject = make_shared<UIObject>();
			uiObject->SetPivot(ePivot::CENTER);
			uiObject->SetScreenPivot(ePivot::LEFTBOT);
			uiObject->SetWidth(150.f);
			uiObject->SetHeight(20.f);
			uiObject->SetPosition(Vec2(100.f, 50));
			uiObject->SetZOrder(500);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// HP바
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"PlayerHPBar", L"..\\Resources\\Texture\\PlayerHPBar.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<UIObject> uiObject = make_shared<PlayerHPBar>();
			uiObject->SetPivot(ePivot::LEFTCENTER);
			uiObject->SetScreenPivot(ePivot::LEFTBOT);
			uiObject->SetWidth(145.f);
			uiObject->SetHeight(15.f);
			uiObject->SetPosition(Vec2(27.5f, 50));
			uiObject->SetZOrder(490);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// HP 텍스트
		{
			auto HPText = make_shared<HPTextObject>();
			HPText->SetScreenPivot(ePivot::LEFTBOT);
			HPText->SetPosition(Vec2(25, -100));
			scene->AddTextObject(HPText);
		}

		// 스킬 아이콘
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
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->LoadSkillIconTexture(GET_SINGLE(UpgradeManager)->GetClass());
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<UIObject> uiObject = make_shared<UIObject>();
			uiObject->SetPivot(ePivot::CENTERBOT);
			uiObject->SetScreenPivot(ePivot::CENTERBOT);
			uiObject->SetWidth(80.f);
			uiObject->SetHeight(80.f);
			uiObject->SetPosition(Vec2(0, 50));
			uiObject->SetZOrder(400);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// 스킬 쿨타임 
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"SkillCoolTime", L"..\\Resources\\Texture\\SkillCool.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<PlayerSkillCoolTimeUI> uiObject = make_shared<PlayerSkillCoolTimeUI>();
			uiObject->SetPivot(ePivot::CENTERTOP);
			uiObject->SetScreenPivot(ePivot::CENTERBOT);
			uiObject->SetWidth(80.f);
			uiObject->SetHeight(0.f);
			uiObject->SetPosition(Vec2(0, 50));
			uiObject->SetZOrder(300);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// 스킬 쿨타임 텍스트
		{
			auto skillCoolTimeText = make_shared<SkillCoolTimeTextObject>();
			skillCoolTimeText->SetBrush("BLACK");
			skillCoolTimeText->SetFormat("30L");
			skillCoolTimeText->SetScreenPivot(ePivot::CENTERBOT);
			skillCoolTimeText->SetPivot(ePivot::CENTER);
			skillCoolTimeText->SetPosition(Vec2(0, -90));
			scene->AddTextObject(skillCoolTimeText);
		}
		// 스킬 키보드 아이콘
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"keyboard_key_q", L"..\\Resources\\Texture\\keyboard_key_q.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<UIObject> uiObject = make_shared<UIObject>();
			uiObject->SetPivot(ePivot::CENTERBOT);
			uiObject->SetScreenPivot(ePivot::CENTERBOT);
			uiObject->SetWidth(40.f);
			uiObject->SetHeight(40.f);
			uiObject->SetPosition(Vec2(0, 5));
			uiObject->SetZOrder(400);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}

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

		// 미니맵
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"MiniMapBase1", L"..\\Resources\\Texture\\minimap_base.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<MiniMapUI> minimap = make_shared<MiniMapUI>();
			minimap->SetPivot(ePivot::RIGHTTOP);
			minimap->SetScreenPivot(ePivot::RIGHTTOP);
			minimap->SetWidth(200.f);
			minimap->SetHeight(200.f);
			minimap->SetPosition(Vec2(0, 0));
			minimap->SetZOrder(500);

			obj->AddComponent(minimap);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);

			// 미니맵 아이콘
			for (int i = 0; i < 50; ++i)
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
					shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"minimap_enemy", L"..\\Resources\\Texture\\minimap_enemy.png");
					shared_ptr<Material> material = make_shared<Material>();
					material->SetShader(shader);
					material->SetTexture(0, texture);

					meshRenderer->SetMaterial(material);
				}
				shared_ptr<UIToggleObject> uiObject = make_shared<UIToggleObject>();
				uiObject->SetToggleKey(KEY_TYPE::F12);
				uiObject->SetPivot(ePivot::CENTER);
				uiObject->SetScreenPivot(ePivot::RIGHTTOP);
				uiObject->SetWidth(8.f);
				uiObject->SetHeight(8.f);
				uiObject->SetPosition(Vec2(80, 80));
				uiObject->SetZOrder(490);

				minimap->addMiniMapIcon(uiObject);
				obj->AddComponent(uiObject);
				obj->AddComponent(meshRenderer);
				scene->AddGameObject(obj);
			}
		}
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"MiniMapBase2", L"..\\Resources\\Texture\\minimap_base_2.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<MiniMapPlayerIcon> uiObject = make_shared<MiniMapPlayerIcon>();
			uiObject->SetPivot(ePivot::RIGHTTOP);
			uiObject->SetScreenPivot(ePivot::RIGHTTOP);
			uiObject->SetWidth(200.f);
			uiObject->SetHeight(200.f);
			uiObject->SetPosition(Vec2(0, 0));
			uiObject->SetZOrder(480);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}

		// 게임오버
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"gameover", L"..\\Resources\\Texture\\gameover.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<GameOverUI> uiObject = make_shared<GameOverUI>();
			uiObject->SetToggleKey(KEY_TYPE::F12);
			uiObject->SetPivot(ePivot::CENTER);
			uiObject->SetScreenPivot(ePivot::CENTER);
			uiObject->SetWidth(1600.f);
			uiObject->SetHeight(900.f);
			uiObject->SetPosition(Vec2(0, 0));
			uiObject->SetZOrder(100);

			obj->AddComponent(uiObject);
			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// 게임오버 텍스트
		{
			auto gameOverText = make_shared<GameOverTextObject>();
			gameOverText->SetFormat("34C");
			gameOverText->SetBrush("WHITE");
			gameOverText->SetText(L"GAME OVER");
			gameOverText->SetPivot(ePivot::CENTER);
			gameOverText->SetScreenPivot(ePivot::CENTER);
			gameOverText->SetPosition(Vec2(0, 0));
			scene->AddTextObject(gameOverText);

		}

	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(25000.f, 10000, 25000));
		light->AddComponent(make_shared<Light>());
		//light->GetLight()->SetLightDirection(Vec3(0.3f, -1.0f, 1.0f));
		light->GetLight()->SetLightDirection(Vec3(-0.1, -1.0f, 0.7));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		light->AddComponent(make_shared<TestCameraScript>());

		scene->AddGameObject(light);
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
			transform->SetLocalPosition(Vec3(2500.0f, 1000.f, 2200.f));
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
			camera->GetTransform()->SetLocalPosition(Vec3(0.f, 195.f, 40.f));
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
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Skill_Tanker.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> gm = gameObjects[idx];

		gm->GetTransform()->SetLocalScale(Vec3(10.f, 5.f, 10.f));
		gm->GetTransform()->SetLocalPosition(Vec3(500, 500.f, 500.f));

		shared_ptr<RigidBody> rbb = make_shared<RigidBody>();
		rbb->SetStatic(true);
		rbb->SetMass(1000000.f);
		gm->AddComponent(rbb);
		gm->AddComponent(make_shared<TankerSkill>());

		gm->AddComponent(make_shared<OrientedBoxCollider>());
		gm->SetCheckFrustum(true);


		Vec3 localPos, localRot, colliderCenter, ColliderSize;
		localPos = Vec3(758.f, 2089.f, -2360.f);
		localRot = Vec3(90.f, 0.f, 0.f);
		colliderCenter = Vec3(0.f, 0.f, 0.f);
		ColliderSize = Vec3(1.46f * 10.f, 0.28f * 5.f, 0.65f * 10.f);

		localPos.x -= 25000.f;
		localPos.z -= 25000.f;
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


		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			gm->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		if (gm->GetCollider()->GetDebugCollider() != nullptr)
			scene->AddGameObject(gm->GetCollider()->GetDebugCollider());

		gm->SetShadow(true);
		scene->AddGameObject(gm);

		scene->GetMainPlayerScript()->SetSkillObject(0, gm);

	}

	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Skill_Healer.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> gm = gameObjects[idx];

		gm->GetTransform()->SetLocalScale(Vec3(50.f, 0.1f, 50.f));
		gm->GetTransform()->SetLocalPosition(Vec3(-100500, 500.f, 500.f));

		shared_ptr<RigidBody> rbb = make_shared<RigidBody>();
		rbb->SetStatic(true);
		rbb->SetMass(1000000.f);
		rbb->SetOverlap();
		gm->AddComponent(rbb);
		gm->AddComponent(make_shared<HealerSkill>());

		gm->AddComponent(make_shared<OrientedBoxCollider>());
		gm->GetCollider()->SetExtent(Vec3(750, 100, 750));
		gm->GetCollider()->SetOffset(Vec3(0, 50, 0));



		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			gm->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		if (gm->GetCollider()->GetDebugCollider() != nullptr)
			scene->AddGameObject(gm->GetCollider()->GetDebugCollider());
		gm->SetShadow(true);
		scene->AddGameObject(gm);

		scene->GetMainPlayerScript()->SetSkillObject(1, gm);

	}

	{
		shared_ptr<GameObject> bomb = GET_SINGLE(Resources)->LoadBombPrefab(Vec3(0, 0, 0));
		bomb->GetRigidBody()->SetOverlap();
		shared_ptr<Bomb> bombScript = dynamic_pointer_cast<Bomb>(bomb->GetMonobehaviour("Bomb"));
		scene->GetMainPlayerScript()->SetBomb(bombScript);
		scene->AddGameObject(bomb);
	}





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


#pragma NetworkPlayerHUD
	{
		// 네트워크 플레이어 체력바
		{
			// 체력바 베이스
			for (int i = 0; i < 2; ++i)
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
					shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"PlayerHPBarBase", L"..\\Resources\\Texture\\PlayerHPBarBase.png");
					shared_ptr<Material> material = make_shared<Material>();
					material->SetShader(shader);
					material->SetTexture(0, texture);

					meshRenderer->SetMaterial(material);
				}
				shared_ptr<NetworkPlayerHPBarBase> uiObject = make_shared<NetworkPlayerHPBarBase>();
				uiObject->SetToggleKey(KEY_TYPE::F12);
				uiObject->SetPivot(ePivot::LEFTCENTER);
				uiObject->SetScreenPivot(ePivot::RIGHTCENTER);
				uiObject->SetNetworkPlayerIndex(i);
				uiObject->SetWidth(100.f);
				uiObject->SetHeight(20.f);
				uiObject->SetPosition(Vec2(-100, 0 - (i * 110)));
				uiObject->SetZOrder(500);

				obj->AddComponent(uiObject);

				obj->AddComponent(meshRenderer);
				scene->AddGameObject(obj);
			}

			// 체력바
			for (int i = 0; i < 2; ++i)
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
					shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"PlayerHPBar", L"..\\Resources\\Texture\\PlayerHPBar.png");
					shared_ptr<Material> material = make_shared<Material>();
					material->SetShader(shader);
					material->SetTexture(0, texture);

					meshRenderer->SetMaterial(material);
				}
				shared_ptr<NetworkPlayerHPBar> uiObject = make_shared<NetworkPlayerHPBar>();
				uiObject->SetToggleKey(KEY_TYPE::F12);
				uiObject->SetPivot(ePivot::LEFTCENTER);
				uiObject->SetScreenPivot(ePivot::RIGHTCENTER);
				uiObject->SetNetworkPlayerIndex(i);
				uiObject->SetWidth(95.f);
				uiObject->SetHeight(15.f);
				uiObject->SetPosition(Vec2(-97.5, 0 - (i * 110)));
				uiObject->SetZOrder(490);

				obj->AddComponent(uiObject);

				obj->AddComponent(meshRenderer);
				scene->AddGameObject(obj);
			}

			// 체력 텍스트
			for (int i = 0; i < 2; ++i)
			{
				auto hpText = make_shared<NetworkPlayerHPTextObject>();
				hpText->SetFormat("15C");
				hpText->SetToggleKey(KEY_TYPE::F12);
				hpText->SetBrush("WHITE");
				hpText->SetPivot(ePivot::CENTER);
				hpText->SetScreenPivot(ePivot::RIGHTCENTER);
				hpText->SetText(L"999/999");
				hpText->SetPosition(Vec2(-50, -3 + (i * 110)));
				hpText->SetNetworkPlayerIdx(i);
				scene->AddTextObject(hpText);

			}

			// 초상화
			for (int i = 0; i < 2; ++i)
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
					shared_ptr<Texture> texture = GET_SINGLE(Resources)->LoadPlayerIconTexture(5);
					shared_ptr<Material> material = make_shared<Material>();
					material->SetShader(shader);
					material->SetTexture(0, texture);

					meshRenderer->SetMaterial(material);
				}
				shared_ptr<NetworkPlayerIcon> uiObject = make_shared<NetworkPlayerIcon>();
				uiObject->SetToggleKey(KEY_TYPE::F12);
				uiObject->SetPivot(ePivot::RIGHTBOT);
				uiObject->SetScreenPivot(ePivot::RIGHTCENTER);
				uiObject->SetNetworkPlayerIndex(i);
				uiObject->SetWidth(50.f);
				uiObject->SetHeight(50.f);
				uiObject->SetPosition(Vec2(0, 10 - (i * 110)));
				uiObject->SetZOrder(500);

				obj->AddComponent(uiObject);

				obj->AddComponent(meshRenderer);
				scene->AddGameObject(obj);
			}
		}

		// 플레이타임
		{
			auto playTimeText = make_shared<PlayTimeTextObject>();
			playTimeText->SetFormat("15C");
			playTimeText->SetBrush("WHITE");
			playTimeText->SetPivot(ePivot::CENTERTOP);
			playTimeText->SetScreenPivot(ePivot::LEFTTOP);
			playTimeText->SetText(L"00:00");
			playTimeText->SetPosition(Vec2(80, 5));
			scene->AddTextObject(playTimeText);
		}
		// 난이도 바
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
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Difficulty_EASY", L"..\\Resources\\Texture\\Difficulty_EASY.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			shared_ptr<DifficultyBar> uiObject = make_shared<DifficultyBar>();
			uiObject->SetPivot(ePivot::CENTER);
			uiObject->SetScreenPivot(ePivot::LEFTTOP);
			uiObject->SetWidth(153.f);
			uiObject->SetHeight(35.f);
			uiObject->SetPosition(Vec2(80, -50));
			uiObject->SetZOrder(200);

			obj->AddComponent(uiObject);

			obj->AddComponent(meshRenderer);
			scene->AddGameObject(obj);
		}
		// 다음 난이도 안내 텍스트
		{
			auto nextDifficultyText = make_shared<TextObject>();
			nextDifficultyText->SetFormat("15C");
			nextDifficultyText->SetBrush("WHITE");
			nextDifficultyText->SetPivot(ePivot::CENTER);
			nextDifficultyText->SetScreenPivot(ePivot::LEFTTOP);
			nextDifficultyText->SetText(L"^");
			nextDifficultyText->SetPosition(Vec2(80, 80));
			scene->AddTextObject(nextDifficultyText);

		}
	}
#pragma endregion

#pragma region Enemy
	{
		for (int i = 0; i < 5; ++i)
		{
			GET_SINGLE(Resources)->LoadEnemyPrefab(i, Vec3(10000 + (i / 2) * 1000.f, 1500.f, 6721 + i % 2 * 1000.f), Vec3(1.2f, 1.2f, 1.2f), 100.f, scene);
		}
	}

	{
		for (int i = 0; i < 5; ++i)
		{
			GET_SINGLE(Resources)->LoadEnemyPrefab(i, Vec3(27328 + (i / 2) * 1000.f, 1500.f, 7446 + i % 2 * 1000.f), Vec3(1.2f, 1.2f, 1.2f), 100.f, scene);

		}
	}

	{
		for (int i = 0; i < 5; ++i)
		{
			GET_SINGLE(Resources)->LoadEnemyPrefab(i, Vec3(28623 + (i / 2) * 1000.f, 1500.f, 15045 + i % 2 * 1000.f), Vec3(1.2f, 1.2f, 1.2f), 100.f, scene);
		}
	}
#pragma endregion

#pragma region LastWaveEnemy
	{
		for (int i = 0; i < 30; ++i)
		{
			GET_SINGLE(Resources)->LoadEnemyPrefab(i % 5, Vec3(-1000000, 1500.f, 0), Vec3(1.2f, 1.2f, 1.2f), 100.f, scene, true);
		}
	}
#pragma endregion

#pragma region Item
	{

		for (int i = 0; i < 17; ++i) {
			GET_SINGLE(Resources)->LoadItemPrefab(i, Vec3(2500.f, 400.f, 3000.f + 100.f * i), scene);
		}


		GET_SINGLE(Resources)->LoadItemPrefab(0, Vec3(27328, 220, 7446), scene);

		GET_SINGLE(Resources)->LoadItemPrefab(1, Vec3(16805, 121, 6721), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(1, Vec3(16755, 121, 6721), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(1, Vec3(16835, 121, 6721), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(1, Vec3(16845, 121, 6721), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(1, Vec3(16855, 121, 6721), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(2, Vec3(28623, 128, 15045), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(3, Vec3(25451, 121, 24415), scene);
		GET_SINGLE(Resources)->LoadItemPrefab(4, Vec3(25651, 121, 24415), scene);
	}



#pragma endregion

#pragma region StagePortal
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Bld_Portal_01.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> gm = gameObjects[0];

		gm->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		gm->GetTransform()->SetLocalPosition(Vec3(0, 0, 0));


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

		// 스테이지 포탈
		localPos = Vec3(-2280.f, 0.f, -11560.f);
		localRot = Vec3(0.f, 0.f, 0.f);
		colliderCenter = Vec3(0.0f, 3.519f, 0.f);
		ColliderSize = Vec3(8.644f, 7.039f, 3.01f);


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

		auto portal = make_shared<StagePortal>();
		gm->AddComponent(portal);
		scene->SetStagePortal(portal);

		scene->AddGameObject(gm);
	}
#pragma endregion

#pragma region test



	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 3; ++i) {
			GET_SINGLE(Resources)->LoadCratePrefab(Vec3(12750, 1500.f + 400.f * j, 15000 + 100 * i + 10 * j), scene);


		}
	}

#pragma endregion 

#pragma region ParticleSystem
	{
		{
			for (int i = 0; i < 20; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::EXPLOSION));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 300.f, 3000.f));
				scene->AddGameObject(particle);
			}
		}
		{
			for (int i = 0; i < 2; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::HEAL));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 300.f, 3000.f));

				scene->AddGameObject(particle);
			}
		}
		{
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_LAUNCHER));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 300.f, 3000.f));

				scene->AddGameObject(particle);
			}
		}
		{
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_PORTAL));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetParent(scene->GetMainCamera()->GetTransform());
				particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));

				scene->AddGameObject(particle);
			}
		}
		{
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_BIRD));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(25000.f, 2000.f, 25000.f));

				scene->AddGameObject(particle);
			}
		}
		{
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_THUNDER));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(25000.f, 2000.f, 25000.f));

				scene->AddGameObject(particle);
			}
			for (int i = 0; i < 35; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_ITEM));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

				scene->AddGameObject(particle);
			}

			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_DUST));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

				scene->AddGameObject(particle);
			}
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_BEAM));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 0.f, 0.f));

				scene->AddGameObject(particle);
			}
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_GATE));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 0.f, 0.f));

				scene->AddGameObject(particle);
			}
			for (int i = 0; i < 1; ++i)
			{
				shared_ptr<GameObject> particle = make_shared<GameObject>();
				particle->AddComponent(make_shared<Transform>());
				particle->AddComponent(make_shared<ParticleSystem>(ParticleType::PARTICLE_GATE_COMP));
				particle->SetCheckFrustum(false);
				particle->GetTransform()->SetLocalPosition(Vec3(-100000.f, 0.f, 0.f));

				scene->AddGameObject(particle);
			}
		}
		scene->SpawnParticle(Vec3(25000.f, 6000.f, 23000.f), ParticleType::PARTICLE_BIRD);
		scene->SpawnParticle(Vec3(25000.f, 2500.f, 23000.f), ParticleType::PARTICLE_THUNDER);
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
