#include "pch.h"
#include "BaseCollider.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"

BaseCollider::BaseCollider(ColliderType colliderType)
	: Component(COMPONENT_TYPE::COLLIDER), m_colliderType(colliderType)
{

}

BaseCollider::~BaseCollider()
{

}

void BaseCollider::setColor(bool color)
{
	m_go->GetMeshRenderer()->GetMaterial()->SetInt(3, color);
}
