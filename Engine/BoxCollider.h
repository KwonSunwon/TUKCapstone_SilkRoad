#pragma once
#include "BaseCollider.h"

class BoxCollider : public BaseCollider
{
public:
	BoxCollider();
	virtual ~BoxCollider();

	virtual void Awake() override;

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) override;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) override;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) override;

	virtual void SetRadius(float radius) override;
	virtual void SetCenter(Vec3 center) override;
	virtual void SetExtent(Vec3 extent) override;
	virtual void SetRotation(Vec3 rotation) override;

	virtual void draw() override;



	const shared_ptr<BoundingBox> GetBoundingBox() { return m_boundingBox; }
private:


	shared_ptr<BoundingBox> m_boundingBox;
	bool m_isIn = false;

};

