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
	MovementStep();
	GetTransform()->SetLocalPosition(m_position);
}

void RigidBody::MovementStep()
{
	m_linearVelocity += m_force * DELTA_TIME;
	m_position += m_linearVelocity * DELTA_TIME;
	m_rotation += m_rotationVelocity * DELTA_TIME;
	m_force = { 0,0,0 };
}



