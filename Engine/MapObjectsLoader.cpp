#include "pch.h"
#include "MapObjectsLoader.h"
#include "Engine.h"
#include "GameObject.h"
#include "MeshData.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OrientedBoxCollider.h"

MapObjectsLoader::MapObjectsLoader()
{

}

MapObjectsLoader::~MapObjectsLoader()
{

}

void MapObjectsLoader::Load(const wstring& path)
{
	std::ifstream jsonFile(path);
	nlohmann::json j;
	shared_ptr<MapObjectInfo> objInfo = make_shared<MapObjectInfo>();

	jsonFile >> j;

	for (const auto& jsonObj : j["objects"])
	{
		objInfo->name = jsonObj["name"].get<std::string>();
		objInfo->meshName = jsonObj["meshName"].get<std::string>();
		objInfo->position = Vec3(jsonObj["position"]["x"].get<float>() + 250.f * 100.f,
			jsonObj["position"]["y"].get<float>(),
			jsonObj["position"]["z"].get<float>() + 250.f * 100.f);
		objInfo->rotation = Vec3(XMConvertToRadians(jsonObj["rotation"]["x"].get<float>()),
			XMConvertToRadians(jsonObj["rotation"]["y"].get<float>() - 180.f),
			XMConvertToRadians(jsonObj["rotation"]["z"].get<float>()));
		objInfo->scale = Vec3(jsonObj["scale"]["x"].get<float>(),
			jsonObj["scale"]["y"].get<float>(),
			jsonObj["scale"]["z"].get<float>());
		objInfo->objectType = jsonObj["objectType"].get<int32>();
		objInfo->boxColliderCenter = Vec3(jsonObj["boxColliderCenter"]["x"].get<float>() * 100.f,
			jsonObj["boxColliderCenter"]["y"].get<float>() * 100.f,
			jsonObj["boxColliderCenter"]["z"].get<float>() * 100.f);
		objInfo->boxColliderExtent = Vec3(jsonObj["boxColliderSize"]["x"].get<float>() * objInfo->scale.x * 100.f / 2.f,
			jsonObj["boxColliderSize"]["y"].get<float>() * objInfo->scale.y * 100.f / 2.f,
			jsonObj["boxColliderSize"]["z"].get<float>() * objInfo->scale.z * 100.f / 2.f);

		std::wstring fbxPath;
		fbxPath.assign(objInfo->meshName.begin(), objInfo->meshName.end());
		fbxPath = L"..\\Resources\\FBX\\" + fbxPath;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(fbxPath);
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[0];

		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalScale(objInfo->scale);
			transform->SetLocalRotation(objInfo->rotation);
			transform->SetLocalPosition(objInfo->position);
			
		}
		if (objInfo->objectType == 1)
		{
			{
				shared_ptr<RigidBody> rb = make_shared<RigidBody>();

				rb->SetStatic(true);
				rb->SetMass(99999.f);
				rb->SetRestitution(0.f);
				go->SetCheckFrustum(false);
				go->AddComponent(rb);
			}
			{
				shared_ptr<OrientedBoxCollider> collider = make_shared<OrientedBoxCollider>();
				collider->SetOffset(objInfo->boxColliderCenter);
				collider->SetExtent(objInfo->boxColliderExtent);
				go->AddComponent(collider);
			}
			if (DEBUG_MODE)
			{
				m_scene->AddGameObject(go->GetCollider()->GetDebugCollider());
			}
		}
		
		go->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

		m_scene->AddGameObject(go);

	}

	jsonFile.close();

}

void MapObjectsLoader::Create(shared_ptr<Scene> scene)
{
	m_scene = scene;
}