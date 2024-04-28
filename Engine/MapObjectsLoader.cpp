#include "pch.h"
#include "MapObjectsLoader.h"
#include "Engine.h"
#include "GameObject.h"
#include "MeshData.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"

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
		objInfo->position = XMFLOAT3(jsonObj["position"]["x"].get<float>() + 35000.f,
			jsonObj["position"]["y"].get<float>(),
			jsonObj["position"]["z"].get<float>() + 35000.f);
		objInfo->rotation = XMFLOAT3(jsonObj["rotation"]["x"].get<float>(),
			jsonObj["rotation"]["y"].get<float>(),
			jsonObj["rotation"]["z"].get<float>());
		objInfo->scale = XMFLOAT3(jsonObj["scale"]["x"].get<float>(),
			jsonObj["scale"]["y"].get<float>(),
			jsonObj["scale"]["z"].get<float>());
		objInfo->objectType = jsonObj["objectType"].get<int32>();
		objInfo->boxColliderCenter = XMFLOAT3(jsonObj["boxColliderCenter"]["x"].get<float>(),
			jsonObj["boxColliderCenter"]["y"].get<float>(),
			jsonObj["boxColliderCenter"]["z"].get<float>());
		objInfo->boxColliderExtent = XMFLOAT3(jsonObj["boxColliderSize"]["x"].get<float>(),
			jsonObj["boxColliderSize"]["y"].get<float>(),
			jsonObj["boxColliderSize"]["z"].get<float>());

		std::wstring fbxPath;
		fbxPath.assign(objInfo->meshName.begin(), objInfo->meshName.end());
		fbxPath = L"..\\Resources\\FBX\\" + fbxPath;
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(fbxPath);
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<GameObject> go = gameObjects[0];

		{
			shared_ptr<Transform> transform = go->GetTransform();
			transform->SetLocalPosition(objInfo->position);
			transform->SetLocalRotation(objInfo->rotation);
			transform->SetLocalScale(objInfo->scale);
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