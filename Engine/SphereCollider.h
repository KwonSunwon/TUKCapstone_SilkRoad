#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void Awake() override;

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) override;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) override;

	virtual void SetRadius(float radius) override;
	virtual void SetCenter(Vec3 center) override;
	virtual void SetExtent(Vec3 extent) override;

	virtual void draw() override;

	

	const shared_ptr<BoundingSphere> GetBoundingSphere() { return m_boundingSphere; }
private:


	shared_ptr<BoundingSphere> m_boundingSphere;
};

