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

	virtual void draw() override;

	void SetRadius(float radius) { m_radius = radius; }
	void SetCenter(Vec3 center) { m_center = center; }


	const shared_ptr<BoundingBox> GetBoundingBox() { return m_boundingBox; }
private:
	// Local ±‚¡ÿ
	float		m_radius = 1.f;
	Vec3		m_center = Vec3(0, 0, 0);

	shared_ptr<BoundingBox> m_boundingBox;

};

