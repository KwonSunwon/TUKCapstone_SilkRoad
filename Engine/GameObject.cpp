#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "MonoBehaviour.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "BaseCollider.h"
#include "Animator.h"
#include "RigidBody.h"
#include "InteractiveObject.h"
#include "NetworkObject.h"

GameObject::GameObject() : Object(OBJECT_TYPE::GAMEOBJECT)
{

}

GameObject::~GameObject()
{

}

void GameObject::Awake()
{
	for(shared_ptr<Component>& component : m_components)
	{
		if(component)
			component->Awake();
	}

	for(shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Awake();
	}
}

void GameObject::Start()
{
	for(shared_ptr<Component>& component : m_components)
	{
		if(component)
			component->Start();
	}

	for(shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Start();
	}
}

void GameObject::Update()
{
	if(IsActive() == false)
		return;
	for(shared_ptr<Component>& component : m_components)
	{
		if(component)
			component->Update();
	}

	for(shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Update();
	}
}

void GameObject::LateUpdate()
{
	if(IsActive() == false)
		return;
	for(shared_ptr<Component>& component : m_components)
	{
		if(component)
			component->LateUpdate();
	}

	for(shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->LateUpdate();
	}
}

void GameObject::FinalUpdate()
{
	if(IsActive() == false)
		return;
	for(shared_ptr<Component>& component : m_components)
	{
		if(component)
			component->FinalUpdate();
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(COMPONENT_TYPE type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	return m_components[index];
}

shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::CAMERA);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<Light> GameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::LIGHT);
	return static_pointer_cast<Light>(component);
}

shared_ptr<ParticleSystem> GameObject::GetParticleSystem()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
	return static_pointer_cast<ParticleSystem>(component);
}

shared_ptr<Terrain> GameObject::GetTerrain()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TERRAIN);
	return static_pointer_cast<Terrain>(component);
}

shared_ptr<BaseCollider> GameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::COLLIDER);
	return static_pointer_cast<BaseCollider>(component);
}

shared_ptr<Animator> GameObject::GetAnimator()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::ANIMATOR);
	return static_pointer_cast<Animator>(component);
}

shared_ptr<RigidBody> GameObject::GetRigidBody()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::RIGIDBODY);
	return static_pointer_cast<RigidBody>(component);
}

shared_ptr<InteractiveObject> GameObject::GetInteractiveObject()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::INTERACTIVE_OBJECT);
	return static_pointer_cast<InteractiveObject>(component);
}

shared_ptr<NetworkObject> GameObject::GetNetworkObject()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::NETWORK);
	return static_pointer_cast<NetworkObject>(component);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if(index < FIXED_COMPONENT_COUNT)
	{
		m_components[index] = component;
	}
	else
	{
		m_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));
	}
}

shared_ptr<MonoBehaviour> GameObject::GetMonobehaviour(string name)
{
	for(auto script : m_scripts) {
		if(script->GetMonovihaviourName() == name) {
			return script;
		}
	}
	return nullptr;
}
