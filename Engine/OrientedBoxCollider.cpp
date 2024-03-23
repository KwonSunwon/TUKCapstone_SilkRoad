#include "pch.h"
#include "OrientedBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Scene.h"

OrientedBoxCollider::OrientedBoxCollider() : BaseCollider(ColliderType::OrientedBox)
{
	m_boundingOrientedBox = make_shared<BoundingOrientedBox>();
	m_go = make_shared<GameObject>();
	m_go->AddComponent(make_shared<Transform>());
	//m_go->GetTransform()->SetLocalPosition(this->GetTransform()->GetLocalPosition());
	Vec3 extents = m_boundingOrientedBox->Extents;
	m_go->GetTransform()->SetLocalScale(extents * 2);//100,200,100


	//m_boundingBox->Center = GetGameObject()->GetTransform()->GetWorldPosition();
	//Vec3 scale = m_go->GetTransform()->GetLocalScale();
	//m_boundingBox->Extents = m_extent/2;

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
		meshRenderer->SetMesh(sphereMesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
		//shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		//material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	m_go->AddComponent(meshRenderer);

}

OrientedBoxCollider::~OrientedBoxCollider()
{

}

void OrientedBoxCollider::Awake()
{




}

void OrientedBoxCollider::FinalUpdate()
{
	shared_ptr<Transform> m_goTransform = m_go->GetTransform();
	shared_ptr<Transform> originTransform = this->GetTransform();

	Vec3 extents = m_boundingOrientedBox->Extents;
	Vec3 roatate = originTransform->GetLocalRotation();
	Vec3 position = originTransform->GetLocalPosition();

	m_goTransform->SetLocalPosition(position);
	m_goTransform->SetLocalScale(extents * 2);
	m_goTransform->SetLocalRotation(roatate);

	m_boundingOrientedBox->Center = GetGameObject()->GetTransform()->GetWorldPosition();
	m_boundingOrientedBox->Orientation = Vec4(XMConvertToRadians(roatate.x),
		XMConvertToRadians(roatate.y),
		XMConvertToRadians(roatate.z),
		1);
}

bool OrientedBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return m_boundingOrientedBox->Intersects(rayOrigin, rayDir, OUT distance);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return m_boundingOrientedBox->Intersects(*boundingSphere);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return m_boundingOrientedBox->Intersects(*boundingBox);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return m_boundingOrientedBox->Intersects(*boundingOrientedBox);
}

void OrientedBoxCollider::SetRadius(float radius)
{
}

void OrientedBoxCollider::SetCenter(Vec3 center)
{
	m_boundingOrientedBox->Center = center;
}

void OrientedBoxCollider::SetExtent(Vec3 extent)
{
	m_boundingOrientedBox->Extents = extent;
}

void OrientedBoxCollider::SetRotation(Vec3 rotation)
{
	m_boundingOrientedBox->Orientation = Vec4(	XMConvertToRadians(rotation.x),
												XMConvertToRadians(rotation.y),
												XMConvertToRadians(rotation.z),
												1);
}

void OrientedBoxCollider::draw()
{

}
