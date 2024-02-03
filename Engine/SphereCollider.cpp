#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Scene.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::Awake()
{
	m_go = make_shared<GameObject>();
	m_go->AddComponent(make_shared<Transform>());
	m_go->GetTransform()->SetLocalPosition(this->GetTransform()->GetLocalPosition()+Vec3(0,50,0));
	m_go->GetTransform()->SetLocalScale(Vec3(200, 200, 200));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	m_go->AddComponent(meshRenderer);
	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_go);
	
}

void SphereCollider::FinalUpdate()
{
	m_go->GetTransform()->SetLocalPosition(this->GetTransform()->GetLocalPosition() + Vec3(0, 75, 0));
	m_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	m_boundingSphere.Radius = m_radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return m_boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}

void SphereCollider::draw()
{

}
