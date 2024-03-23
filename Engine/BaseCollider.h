#pragma once
#include "Component.h"
static int next_id = 0;

enum class ColliderType
{
	Sphere,
	Box,
	OrientedBox,
};

class BaseCollider : public Component, public std::enable_shared_from_this<BaseCollider>
{
private:
	
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual void Awake() override {};

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) = 0;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) = 0;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) = 0;
	virtual void draw() {};

	ColliderType GetColliderType() { return m_colliderType; }

	virtual void SetRadius(float radius) = 0;
	virtual void SetCenter(Vec3 center) = 0;
	virtual void SetExtent(Vec3 extent) = 0;
	virtual void SetRotation(Vec3 rotation) = 0;

	void setColor(Vec4 color, bool active);

	int GetColliderId() { return m_id; }

	void UpdateNodePos();
	bool updatePos = false;

public:
	ColliderType m_colliderType = {};
	

	int m_id;
	
	
public:
	shared_ptr<GameObject> m_go;
};