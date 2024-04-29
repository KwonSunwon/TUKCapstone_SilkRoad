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
	//m_inertia
	m_position = GetTransform()->GetLocalPosition();
	m_baseCollider = GetGameObject()->GetCollider();
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
	m_baseCollider->SetCenter(m_position);
	m_baseCollider->UpdateNodePos();
}

void RigidBody::MoveTo(Vec3 position)
{
	m_position = position;
	m_baseCollider->SetCenter(m_position);
	m_baseCollider->UpdateNodePos();
}

void RigidBody::MovementStep(int iterations)
{

	

	if (m_isStatic)
		return;

	if (m_position.x > 50000.f) {
		m_position.x = 1000;
		m_position.y = 2000;
	}
		


	if (m_position.z > 50000.f) {
		m_position.z = 1000;
		m_position.y = 2000;
	}
		

	if (m_position.x < 0) {
		m_position.x = 50000;
		m_position.y = 2000;
	}

		

	if (m_position.z < 0) {
		m_position.z = 50000;
		m_position.y = 2000;
	}

	if (m_position.y < 0) {
		m_position.y = 2000;
	}
		
	


	float normalForce = m_mass * -m_gravity.y;
	float frictionCoef = 2.f; //¸¶Âû°è¼ö
	float friction = frictionCoef * normalForce;

	Vec3 frictionDir = -m_linearVelocity;
	float mag = frictionDir.Length();
	if (mag > FLT_EPSILON)
	{
		frictionDir /=  mag;

		Vec3 frictionForce = frictionDir * friction;
		Vec3 frictionAcc = frictionForce / m_mass;
		

		Vec3 resultVel = m_linearVelocity + frictionAcc * DELTA_TIME;

		if (resultVel.x * m_linearVelocity.x < 0.f)
		{
			m_linearVelocity.x = 0;
		}
		else
		{
			m_linearVelocity.x = resultVel.x;
		}

		if (resultVel.y * m_linearVelocity.y < 0.f)
		{
			//m_linearVelocity.y = 0;
		}
		else
		{
			//m_linearVelocity.y = resultVel.y;
		}

		if (resultVel.z * m_linearVelocity.z < 0.f)
		{
			m_linearVelocity.z = 0;
		}
		else
		{
			m_linearVelocity.z = resultVel.z;
		}


	}
	Vec3 acc = m_force / m_mass;
	m_linearVelocity += acc * DELTA_TIME / (float)iterations;
	m_linearVelocity += m_gravity * DELTA_TIME / (float)iterations;
	if (m_linearVelocity.LengthSquared() > m_maxSpeed * m_maxSpeed) {
		m_linearVelocity.Normalize();
		m_linearVelocity *= m_maxSpeed;
	}

	m_position += m_linearVelocity * DELTA_TIME / (float)iterations;
	m_rotation += m_rotationVelocity * DELTA_TIME / (float)iterations;
	
	
	m_force = Vec3{ 0,0,0 };

	


	m_baseCollider->SetCenter(m_position);
	if (m_linearVelocity.LengthSquared() > 0.f) {
		m_baseCollider->UpdateNodePos();
	}
}



