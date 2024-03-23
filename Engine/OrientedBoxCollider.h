#pragma once
#include "BaseCollider.h"

class OrientedBoxCollider : public BaseCollider
{
public:
	OrientedBoxCollider();
	virtual ~OrientedBoxCollider();

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



	const shared_ptr<BoundingOrientedBox> GetBoundingOrientedBox() { return m_boundingOrientedBox; }
private:


	shared_ptr<BoundingOrientedBox> m_boundingOrientedBox;
	bool m_isIn = false;

};

