#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	Box,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual void Awake() override {};

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) = 0;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) = 0;
	virtual void draw() {};

	ColliderType GetColliderType() { return m_colliderType; }

	void setColor(bool color);
	

private:
	ColliderType m_colliderType = {};
	

protected:
	shared_ptr<GameObject> m_go;
};