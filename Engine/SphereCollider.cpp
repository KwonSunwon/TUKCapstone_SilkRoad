#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{
	m_boundingSphere = make_shared<BoundingSphere>();
	if (DEBUG_MODE) {
		m_go = make_shared<GameObject>();
		m_go->AddComponent(make_shared<Transform>());
		m_go->GetTransform()->SetLocalScale(Vec3(m_boundingSphere->Radius * 2, m_boundingSphere->Radius * 2, m_boundingSphere->Radius * 2));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			meshRenderer->SetMaterial(material);
		}
		m_go->AddComponent(meshRenderer);
	}
}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::Awake()
{
	SetCenter(GetGameObject()->GetTransform()->GetLocalPosition());
}

void SphereCollider::FinalUpdate()
{
	if (DEBUG_MODE)
	{
		m_go->GetTransform()->SetLocalPosition(m_boundingSphere->Center);
		float radius = m_boundingSphere->Radius;
		m_go->GetTransform()->SetLocalScale(Vec3(radius * 2, radius * 2, radius * 2));
	}

}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return m_boundingSphere->Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return m_boundingSphere->Intersects(*boundingSphere);
}

bool SphereCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return m_boundingSphere->Intersects(*boundingBox);
}

bool SphereCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return m_boundingSphere->Intersects(*boundingOrientedBox);
}

void SphereCollider::SetRadius(float radius)
{
	m_boundingSphere->Radius = radius;
}

void SphereCollider::SetCenter(Vec3 center)
{
	m_boundingSphere->Center = center + m_offset;
}

void SphereCollider::SetExtent(Vec3 extent)
{
}

void SphereCollider::SetRotation(Vec3 rotation)
{
}

void SphereCollider::draw()
{

}
