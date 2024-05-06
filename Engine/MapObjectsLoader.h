#pragma once
#include "scene.h"

struct MapObjectInfo
{
	std::string name;
	std::string	meshName;
	Vec3	position;
	Vec3	rotation;
	Vec3	scale;
	int32	objectType;	// 1 : �浹����, 2: �浹 �Ұ�
	Vec3	boxColliderCenter;
	Vec3	boxColliderExtent;
};

class MapObjectsLoader
{
public:
	MapObjectsLoader();
	virtual ~MapObjectsLoader();

	void Load(const wstring& path);
	void Create(shared_ptr<Scene> scene);

private:
	shared_ptr<Scene> m_scene;
};