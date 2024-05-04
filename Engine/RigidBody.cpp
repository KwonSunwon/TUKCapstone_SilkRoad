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
#include "Manifold.h"



RigidBody::RigidBody() : Component(COMPONENT_TYPE::RIGIDBODY)
{
	
}

RigidBody::~RigidBody()
{

}

void RigidBody::Awake()
{
	//m_inertia
	m_manifolds = make_shared<vector<shared_ptr<Manifold>>>();
	m_manifolds->reserve(100);
	m_position = GetTransform()->GetLocalPosition();
	m_baseCollider = GetGameObject()->GetCollider();
	//m_gameTerrain = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain->GetTerrain();
}

void RigidBody::FinalUpdate()
{
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


	if (m_position.y < 0) {
		m_position.y = 2000;
	}
	
	ApplyFriction(iterations);
	ApplyAccVel(iterations);

	

	m_baseCollider->SetCenter(m_position);
	if (m_linearVelocity.LengthSquared() > 0.f) {
		m_baseCollider->UpdateNodePos();
	}
}

void RigidBody::AddCollideEvent(shared_ptr<Manifold> event)
{
	m_manifolds->push_back(event);
}

void RigidBody::ApplyFriction(int iterations)
{
	if (m_frictionCoef < FLT_EPSILON)
		return;



	float normalForce = m_mass * -m_gravity.y;
	float friction = m_frictionCoef * normalForce;

	Vec3 frictionDir = -m_linearVelocity;
	float mag = frictionDir.Length();

	if (mag < FLT_EPSILON)
		return;




	frictionDir /= mag;

	Vec3 frictionForce = frictionDir * friction;
	Vec3 frictionAcc = frictionForce / m_mass;
	Vec3 resultVel = m_linearVelocity + frictionAcc * DELTA_TIME / (float)iterations;

	if (resultVel.x * m_linearVelocity.x < 0.f) {
		m_linearVelocity.x = 0;
	}
	else {
		m_linearVelocity.x = resultVel.x;
	}


	if (resultVel.z * m_linearVelocity.z < 0.f) {
		m_linearVelocity.z = 0;
	}
	else {
		m_linearVelocity.z = resultVel.z;
	}
}

void RigidBody::ApplyAccVel(int iterations)
{

	Vec3 acc = m_force / m_mass;
	m_linearVelocity += acc * DELTA_TIME;
	if (m_useGravity)
		m_linearVelocity += m_gravity * DELTA_TIME / (float)iterations;

	Vec3 XZVel = GetXZVelocity();
	if (XZVel.LengthSquared() > m_maxSpeed * m_maxSpeed) {
		XZVel.Normalize();
		XZVel *= m_maxSpeed;
		m_linearVelocity.x = XZVel.x;
		m_linearVelocity.z = XZVel.z;
	}
	if (m_linearVelocity.y > m_maxAirSpeed) {
		m_linearVelocity.y = m_maxAirSpeed;
	}

	m_position += m_linearVelocity * DELTA_TIME / (float)iterations;
	m_rotation += m_rotationVelocity * DELTA_TIME / (float)iterations;


	m_force = Vec3{ 0,0,0 };
}




