#include "pch.h"
#include "Resources.h"
#include "Engine.h"
#include "MeshData.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Item.h"
#include "Transform.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "OrientedBoxCollider.h"
#include "Timer.h"
#include "MeshRenderer.h"
#include "Bomb.h"
#include "StagePortal.h"
#include "InteractiveObject.h"
#include "Enemy.h"
#include "EnemyHP.h"
#include "Scene.h"
void Resources::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
	CreateDefaultGameObject();
}

shared_ptr<Mesh> Resources::LoadPointMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Point");
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(1);
	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(1);
	idx[0] = 0;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Point", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Rectangle", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Cube", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
	/*if (findMesh)
		return findMesh;*/

	float radius = 0.5f; // 구의 반지름
	uint32 stackCount = 20; // 가로 분할
	uint32 sliceCount = 20; // 세로 분할

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Sphere", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadTerrainMesh(int32 sizeX, int32 sizeZ)
{
	vector<Vertex> vec;

	for (int32 z = 0; z < sizeZ + 1; z++)
	{
		for (int32 x = 0; x < sizeX + 1; x++)
		{
			Vertex vtx;
			vtx.pos = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			vtx.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			vtx.normal = Vec3(0.f, 1.f, 0.f);
			vtx.tangent = Vec3(1.f, 0.f, 0.f);

			vec.push_back(vtx);
		}
	}

	vector<uint32> idx;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z)+(x));
			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z + 1) + (x + 1));
		}
	}

	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Terrain");
	if (findMesh)
	{
		findMesh->Create(vec, idx);
		return findMesh;
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Terrain", mesh);
	return mesh;
}

shared_ptr<GameObject> Resources::LoadItemPrefab(int id, Vec3 location)
{
#pragma region Item
	{
		int idx = 0;
		shared_ptr<MeshData> meshData = make_shared<MeshData>();
		Vec3 scale = Vec3(1.f, 1.f, 1.f);
		Vec3 rotation = Vec3(0.f, 0.f, 0.f);
		switch (id) {
		case 0:
			 meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Bomb.fbx");
			 break;

		case 1:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_BatteryPack_02.fbx");
			break;

		case 2:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Cylinder.fbx");
			scale = Vec3(2, 2, 2);
			break;

		case 3:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Box.fbx");
			break;

		case 4:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Ham.fbx");
			break;

		case 5:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Axe.fbx");
			scale = Vec3(1.5f, 1.5f, 1.5f);
			break;

		case 6:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Cola_Can.fbx");
			scale = Vec3(4, 4, 4);
			break;

		case 7:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Food.fbx");
			scale = Vec3(3, 3, 3);
			break;

		case 8:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Hammer.fbx");
			scale = Vec3(1.5f, 1.5f, 1.5f);
			break;

		case 9:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Pouch.fbx");
			scale = Vec3(4, 4, 4);
			break;

		case 10:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Fork.fbx");
			scale = Vec3(4, 4, 4);
			rotation = Vec3(3.14 / 2, 0, 0);
			break;

		case 11:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Knife.fbx");
			scale = Vec3(3, 3, 3);
			rotation = Vec3(3.14 / 2, 0, 0);
			break;

		case 12:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_PDA.fbx");
			scale = Vec3(3, 3, 3);
			rotation = Vec3(3.14 / 2, 0, 0);
			break;

		case 13:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Scope.fbx");
			scale = Vec3(4, 4, 4);
			break;

		case 14:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_SwipeCard.fbx");
			scale = Vec3(7, 7, 7);
			break;

		case 15:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Sword.fbx");
			scale = Vec3(1.5f, 1.5f, 1.5f);
			break;

		case 16:
			meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SM_Item_Helmet.fbx");
			scale = Vec3(2, 2, 2);
			break;

		default:
			break;
		}
		
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[idx];

		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(location);
			transform->SetLocalScale(scale);
			transform->SetLocalRotation(rotation);
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetUseGravity(false);
			rb->SetOverlap();
			go->SetCheckFrustum(true);
			go->AddComponent(rb);
		}

		//콜라이더 설정 
		//콜라이더의 위치,회전은 Gameobject의 Transform을 사용
		{
			shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			collider->SetRadius(100.f);

			collider->SetOffset(Vec3(0, 50, 0));
			go->AddComponent(collider);
		}



		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		//추가적인 컴포넌트 부착
		{
			shared_ptr<Item> item = make_shared<Item>();
			item->SetItemId(id);
			go->AddComponent(item);
			go->SetShadow(true);
			
		}
		return go;
	}



#pragma endregion
}

void Resources::LoadEnemyPrefab(int modelNum, Vec3 Location, Vec3 Scale, float hp, shared_ptr<Scene> scene)
{
	int idx = 0;
	shared_ptr<MeshData> meshData;
	switch (modelNum)
	{
	case 0:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Enemy_Spike.fbx");
		break;

	case 1:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Enemy_Ice.fbx");
		break;

	case 2:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Enemy_Armor.fbx");
		break;

	case 3:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Enemy_Cocker.fbx");
		break;

	case 4:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Enemy_Combat.fbx");
		break;

	default:
		break;
	}

	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
	shared_ptr<GameObject> go = gameObjects[0];

	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Noise", L"..\\Resources\\Texture\\noiseTex2.jpg");
	go->GetMeshRenderer()->GetMaterial()->SetTexture(2, texture);

	//Transform 설정
	{
		shared_ptr<Transform> transform = go->GetTransform();
		transform->SetLocalPosition(Location);
		transform->SetLocalScale(Scale);
	}

	//강체 설정
	{
		shared_ptr<RigidBody> rb = make_shared<RigidBody>();

		rb->SetStatic(false);
		rb->SetMass(15000.f);
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
		collider->SetOffset(Vec3(0, 100, 0));
		go->AddComponent(collider);
	}

	//디버그용 콜라이더 매쉬 설정
	/*if (DEBUG_MODE)
	{
		scene->AddGameObject(go->GetCollider()->GetDebugCollider());
	}*/

	//Instancing 유무 설정(사용:0,0  미사용:0,1)
	{
		go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
	}

	//추가적인 컴포넌트 부착
	{
		shared_ptr<Enemy> enemyScript = make_shared<Enemy>();
		enemyScript->AddPlayer(scene->GetPlayers()[0]);
		enemyScript->AddPlayer(scene->GetPlayers()[GUEST_PLAYER1]);
		enemyScript->AddPlayer(scene->GetPlayers()[GUEST_PLAYER2]);
		go->AddComponent(enemyScript);
		go->SetShadow(true);
		int networkId = scene->AddNetworkObject();
		enemyScript->SetNetworkId(networkId);
		scene->m_enemies[networkId] = enemyScript;

		//hpbar
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->AddComponent(make_shared<Transform>());
			obj->GetTransform()->SetLocalScale(Vec3(100.f, 10.f, 100.f));

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
				meshRenderer->SetMesh(mesh);
			}
			{
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Hpbar", L"..\\Resources\\Texture\\hpbar.png");
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);

				meshRenderer->SetMaterial(material);
			}
			obj->AddComponent(meshRenderer);
			shared_ptr<EnemyHP> enemyHP = make_shared<EnemyHP>();
			enemyHP->SetParentEnemy(enemyScript);
			
			obj->AddComponent(enemyHP);
			scene->AddGameObject(obj);

		}
	}
	scene->AddGameObject(go);

}

shared_ptr<GameObject> Resources::LoadBombPrefab(Vec3 Location)
{
#pragma region Bomb
	{
		shared_ptr<GameObject> go = make_shared<GameObject>();
		//Resource에서 메쉬 로드용
		{

			go->AddComponent(make_shared<Transform>());
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadSphereMesh();
				meshRenderer->SetMesh(mesh);
			}
			{
				meshRenderer->SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GameObject"));
			}
			go->AddComponent(meshRenderer);
		}

		//Transform 설정
		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(Vec3(15000.f, 1500.f, 2000.f));
		}

		//강체 설정
		{
			shared_ptr<RigidBody> rb = make_shared<RigidBody>();

			rb->SetStatic(true);
			rb->SetMass(100000.f);
			rb->SetRestitution(0.f);
			go->SetCheckFrustum(false);
			go->AddComponent(rb);
		}

		//콜라이더 설정 
		//콜라이더의 위치,회전은 Gameobject의 Transform을 사용
		{
			//Sphere를 사용할경우 이곳의 주석을 풀어서 사용
			shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			collider->SetRadius(100.f);
			go->AddComponent(collider);
		}


		//Instancing 유무 설정(사용:0,0  미사용:0,1)
		{
			go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		}

		//추가적인 컴포넌트 부착
		{
			shared_ptr<Bomb> bomb = make_shared<Bomb>();
			bomb->SetMonovihaviourName("Bomb");
			go->AddComponent(bomb);
		}
		return go;

	}
}

shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor, uint32 arraysize)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor, arraysize);
	Add(name, texture);

	return texture;
}

shared_ptr<Texture> Resources::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->CreateFromResource(tex2D);
	Add(name, texture);

	return texture;
}

shared_ptr<MeshData> Resources::LoadFBX(const wstring& path)
{
	wstring key = path;

	shared_ptr<MeshData> meshData = Get<MeshData>(key);
	if (meshData)
		return meshData->Clone();
		//return meshData;

	meshData = MeshData::LoadFromFBX(path);
	meshData->SetName(key);
	Add(key, meshData);

	return meshData;
}

void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skybox.fx", info);
		Add<Shader>(L"Skybox", shader);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred", shader);
	}

	// WireFrame (WireFrame)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::WIREFRAME
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"WireFrame", shader);
	}

	// Forward (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"Texture", shader);
	}

	// Alpha Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"AlphaTexture", shader);
	}

	// DirLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_DirLight",
			"",
			"",
			"",
			"PS_DirLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"DirLight", shader);
	}

	// PointLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_PointLight",
			"",
			"",
			"",
			"PS_PointLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"PointLight", shader);
	}

	// Final
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_Final",
			"",
			"",
			"",
			"PS_Final"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"Final", shader);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeShader", shader);
	}

	// Particle
	{
		ShaderInfo info =
		{
			SHADER_TYPE::PARTICLE,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\particle.fx", info, arg);
		Add<Shader>(L"Particle", shader);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\particle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeParticle", shader);
	}

	// Shadow
	{
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shadow.fx", info, arg);
		Add<Shader>(L"Shadow", shader);
	}

	// Tessellation
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\tessellation.fx", info, arg);
		Add<Shader>(L"Tessellation", shader);
	}

	// Terrain
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_BACK,
			//RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\terrain.fx", info, arg);
		Add<Shader>(L"Terrain", shader);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\animation.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeAnimation", shader);
	}
}

void Resources::CreateDefaultMaterial()
{
	// Skybox
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Skybox", material);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DirLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		Add<Material>(L"DirLight", material);
	}

	// PointLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PointLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"PointLight", material);
	}

	// Final
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Final");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseLightTarget"));
		material->SetTexture(2, GET_SINGLE(Resources)->Get<Texture>(L"SpecularLightTarget"));
		Add<Material>(L"Final", material);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"ComputeShader", material);
	}

	// Particle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Particle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Particle", material);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeParticle", material);
	}

	// GameObject
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Leather", L"..\\Resources\\Texture\\Leather.jpg");
		//shared_ptr<Texture> texture2 = GET_SINGLE(Resources)->Load<Texture>(L"Leather_Normal", L"..\\Resources\\Texture\\Leather_Normal.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		//material->SetTexture(1, texture2);
		Add<Material>(L"GameObject", material);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Shadow");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Shadow", material);
	}

	// Tessellation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Tessellation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Tessellation", material);
	}

	// Terrain
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Terrain");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Terrain", L"..\\Resources\\Texture\\Terrain\\terrain.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		Add<Material>(L"Terrain", material);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeAnimation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeAnimation", material);
	}

}

void Resources::CreateDefaultGameObject()
{

}
