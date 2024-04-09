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
	m_gameTerrain = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain->GetTerrain();
}

void RigidBody::FinalUpdate()
{
	MovementStep();
	GetTransform()->SetLocalPosition(m_position);
}

void RigidBody::MovementStep()
{
	if (m_isStatic)
		return;
	//float y = m_gameTerrain->getHeight(m_position.x, m_position.z);
	//test position reset
	{
		if (m_position.x > 25000)
			m_position.x = 0;

		if (m_position.z > 25000.f)
			m_position.z = 0;

		if (m_position.x < 0)
			m_position.x = 25000;

		if (m_position.z < 0)
			m_position.z = 25000;

		if (m_position.y < -1000)
		{
			m_position.y = 2000;
			m_linearVelocity.y = 0;
		}


	}

	Vec3 acc = m_force / m_mass;
	m_linearVelocity += acc * DELTA_TIME;

	m_linearVelocity += m_gravity * DELTA_TIME;

	m_position += m_linearVelocity * DELTA_TIME;
	m_rotation += m_rotationVelocity * DELTA_TIME;
	m_force = { 0,0,0 };

	

	
}



