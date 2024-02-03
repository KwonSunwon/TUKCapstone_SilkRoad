#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual void Awake() override {};

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual void draw() {};
	

private:
	ColliderType m_colliderType = {};
	

protected:
	shared_ptr<GameObject> m_go;
};