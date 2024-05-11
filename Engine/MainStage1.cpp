#include "pch.h"
#include "MainStage1.h"
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

#include "Network.h"
#include "NetworkPlayer.h"

shared_ptr<class Scene> LoadMainScene()
{
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

		loader->Load(L"..\\Resources\\MapData\\ExportedObjects.json");
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
		obj->AddComponent(meshRenderer);
		//scene->AddGameObject(obj);
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

#pragma region Characters Setting Example
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Characters.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[idx];
		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(Vec3(2500.f, 1500.f, 2500.f));
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
			camera->GetTransform()->SetLocalPosition(Vec3(0.f, 140.f, 40.f));
			camera->GetTransform()->SetLocalRotation(Vec3(XMConvertToRadians(10.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
		}

		//플레이어 컴포넌트 부착
		{
			shared_ptr<Player> playerScript = make_shared<Player>();
			// 총알 오브젝트 풀 생성
			for (int i = 0; i < 20; ++i)
			{
				shared_ptr<GameObject> bullet = make_shared<GameObject>();
				bullet->SetName(L"Bullet");

				{
					bullet->AddComponent(make_shared<Transform>());
					bullet->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
					bullet->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
				}

				shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
				{
					shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadSphereMesh();
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
				bullet->AddComponent(meshRenderer);

				{
					shared_ptr<RigidBody> rb = make_shared<RigidBody>();
					rb->SetStatic(true);
					rb->SetRestitution(0.f);
					bullet->SetCheckFrustum(false);
					bullet->AddComponent(rb);
				}
				{
					//shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
					//collider->SetRadius(10.f);
					shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
					collider->SetExtent(Vec3(10, 10, 10));
					bullet->AddComponent(collider);
				}
				{
					//bullet->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
				}
				bullet->SetActive(true);

				shared_ptr<PlayerBullet> bulletScript = make_shared<PlayerBullet>();
				playerScript->AddBullet(bulletScript);

				bullet->AddComponent(bulletScript);
				
				shared_ptr<GameObject> bomb = GET_SINGLE(Resources)->LoadBombPrefab(Vec3(0,0,0));
				shared_ptr<Bomb> bombScript= dynamic_pointer_cast<Bomb>(bomb->GetMonobehaviour("Bomb"));
				bulletScript->SetBomb(bombScript);
				scene->AddGameObject(bullet);
				scene->AddGameObject(bomb);

			}
			playerScript->SetPlayerCamera(scene->GetMainCamera());
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
		for (int i = 0; i < 1; ++i)
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

			//강체 설정
			{
				shared_ptr<RigidBody> rb = make_shared<RigidBody>();

				rb->SetStatic(true);
				rb->SetMass(150.f);
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



				collider->SetOffset(Vec3(0, 100, 0));
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
				shared_ptr<Enemy> enemyScript = make_shared<Enemy>();
				enemyScript->AddPlayer(scene->GetPlayers()[0]);
				go->AddComponent(enemyScript);
				//go->AddComponent(make_shared<PlayerAnimation>());
			}

			scene->AddGameObject(go);
		}
	}
#pragma endregion


#pragma region Item
	{
		scene->AddGameObject(GET_SINGLE(Resources)->LoadItemPrefab(0, Vec3(3000, 300, 5000)));
		scene->AddGameObject(GET_SINGLE(Resources)->LoadItemPrefab(0, Vec3(4000, 300, 5000)));
	}



#pragma endregion



//#pragma region test
//
//
//
//	for (int j = 0; j < 5; ++j) {
//		for (int i = 0; i < 10; ++i) {
//
//
//			shared_ptr<GameObject> gm = make_shared<GameObject>();
//			gm->AddComponent(make_shared<Transform>());
//			gm->GetTransform()->SetLocalScale(Vec3(150.f, 100.f, 100.f));
//			gm->GetTransform()->SetLocalPosition(Vec3(12500 + 100 * i, 1500.f + 400.f * i, 15000+ 100*j));
//
//			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
//			{
//				shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadCubeMesh();
//				meshRenderer->SetMesh(mesh);
//			}
//
//			{
//				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
//				shared_ptr<Material> material = make_shared<Material>();
//				material->SetShader(shader);
//
//				material->SetInt(3, 1);
//				material->SetVec4(3, Vec4(1, 1, 1, 1));
//				meshRenderer->SetMaterial(material);
//			}
//			gm->AddComponent(meshRenderer);
//
//			gm->AddComponent(make_shared<RigidBody>());
//			//gm->AddComponent(make_shared<TestDragon>());
//
//			if (i & 1) {
//				gm->AddComponent(make_shared<OrientedBoxCollider>());
//				gm->GetCollider()->SetExtent(Vec3(75, 50, 50));
//
//
//				/*gm->AddComponent(make_shared<SphereCollider>());
//				gm->GetCollider()->SetRadius(100.f);*/
//
//
//			}
//			else {
//				gm->AddComponent(make_shared<SphereCollider>());
//				gm->GetCollider()->SetRadius(100.f);
//
//				/*gm->AddComponent(make_shared<OrientedBoxCollider>());
//				gm->GetCollider()->SetExtent(Vec3(75, 50, 50));*/
//
//
//			}
//
//			//Instancing 유무 설정(사용:0,0  미사용:0,1)
//			{
//				gm->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);
//			}
//
//			if (gm->GetCollider()->GetDebugCollider() != nullptr)
//				scene->AddGameObject(gm->GetCollider()->GetDebugCollider());
//			scene->AddGameObject(gm);
//		}
//	}
//
//#pragma endregion 


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
