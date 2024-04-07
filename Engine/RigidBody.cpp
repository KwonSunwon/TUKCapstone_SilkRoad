#include "pch.h"
#include "RigidBody.h"
#include "Engine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Terrain.h"

RigidBody::RigidBody() : Component(COMPONENT_TYPE::RIGIDBODY)
{
	
}

RigidBody::~RigidBody()
{

}

void RigidBody::Awake()
{
	m_position = GetTransform()->GetLocalPosition();
}

void RigidBody::FinalUpdate()
{
	GetTransform()->SetLocalPosition(m_position);
}



