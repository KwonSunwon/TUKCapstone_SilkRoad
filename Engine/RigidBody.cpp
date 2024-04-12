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
	//m_gameTerrain = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain->GetTerrain();
}

void RigidBody::FinalUpdate()
{
	//MovementStep();
	GetTransform()->SetLocalPosition(m_position);
}

void RigidBody::Move(Vec3 amount)
{
	m_position += amount;
	GetGameObject()->GetCollider()->UpdateNodePos();
}

void RigidBody::MoveTo(Vec3 position)
{
	m_position = position;
	GetGameObject()->GetCollider()->UpdateNodePos();
}

void RigidBody::MovementStep(int iterations)
{

	

	if (m_isStatic)
		return;

	//test position reset
	if(DEBUG_MODE)
	{
		if (m_position.x > 50000.f)
			m_position.x = 0;

		if (m_position.z > 50000.f)
			m_position.z = 0;

		if (m_position.x < 0)
			m_position.x = 50000;

		if (m_position.z < 0)
			m_position.z = 50000;

		if (m_position.y < -1000)
		{
			m_position.y = 2000;
			m_linearVelocity.y = 0;
		}


	}

	Vec3 acc = m_force / m_mass;
	m_linearVelocity += acc * DELTA_TIME / (float)iterations;

	m_linearVelocity += m_gravity * DELTA_TIME / (float)iterations;

	m_position += m_linearVelocity * DELTA_TIME / (float)iterations;
	m_rotation += m_rotationVelocity * DELTA_TIME / (float)iterations;
	GetGameObject()->GetCollider()->SetCenter(m_position);
	
	m_force = Vec3{ 0,0,0 };

	

	if (m_linearVelocity.LengthSquared() > 0.f) {
		GetGameObject()->GetCollider()->UpdateNodePos();
	}
}



