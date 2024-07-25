#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "RigidBody.h"
#include "InteractiveObject.h"

Component::Component(COMPONENT_TYPE type) : Object(OBJECT_TYPE::COMPONENT), m_type(type)
{

}

Component::~Component()
{
}

shared_ptr<GameObject> Component::GetGameObject()
{
	return m_gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return m_gameObject.lock()->GetTransform();
}

shared_ptr<MeshRenderer> Component::GetMeshRenderer()
{
	return m_gameObject.lock()->GetMeshRenderer();
}

shared_ptr<Animator> Component::GetAnimator()
{
	return m_gameObject.lock()->GetAnimator();
}

shared_ptr<RigidBody> Component::GetRigidBody()
{
	return m_gameObject.lock()->GetRigidBody();
}

shared_ptr<InteractiveObject> Component::GetInteractiveObject()
{
	return m_gameObject.lock()->GetInteractiveObject();
}

shared_ptr<NetworkObject> Component::GetNetworkObject()
{
	return m_gameObject.lock()->GetNetworkObject();
}
