#include "pch.h"
#include "BaseCollider.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"

BaseCollider::BaseCollider(ColliderType colliderType)
	: Component(COMPONENT_TYPE::COLLIDER), m_colliderType(colliderType)
{
	m_id = next_id++;
}

BaseCollider::~BaseCollider()
{

}

void BaseCollider::setColor(Vec4 color,bool active)
{
	m_go->GetMeshRenderer()->GetMaterial()->SetInt(3, active);
	m_go->GetMeshRenderer()->GetMaterial()->SetVec4(3, color);
}

void BaseCollider::UpdateNodePos()
{
	updatePos = true;
}
