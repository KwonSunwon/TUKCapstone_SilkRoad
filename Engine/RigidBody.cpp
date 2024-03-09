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

}

void RigidBody::FinalUpdate()
{
	if (m_isKinematic)
		return;

	applyGravity();
	applyDrag();
	applyFriction();
	updatePosition();
}

void RigidBody::applyGravity()
{
	if (!m_useGravity)
		return;

	accelerate(m_gravity);
}

void RigidBody::applyFriction()
{
	float normalForce = m_mass * m_gravity.y;
	float friction = m_frictionCoef * normalForce;
	Vec3 frictionDir = m_velocity;
	float mag = frictionDir.Length();
	if (mag > FLT_EPSILON) {
		frictionDir /= mag;
		Vec3 frictionForce = frictionDir * friction;
		Vec3 frictionAcc = frictionForce / m_mass;
		frictionAcc.y = 0;
		accelerate(frictionAcc);
	}
	 
}

void RigidBody::applyDrag()
{
	float dragMag = m_velocity.LengthSquared() * m_drag;
	Vec3 dragDir = m_velocity;
	dragDir.Normalize();
	dragDir *= -dragMag;
	addForce(dragDir, FORCEMODE::IMPULSE);
}

void RigidBody::updatePosition()
{
	m_position = GetTransform()->GetLocalPosition();
	m_priorPosition = m_position;
	m_velocity += m_acceleration * DELTA_TIME;
	m_position += m_velocity * DELTA_TIME;
	m_acceleration = {};
	float y = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain->GetTerrain()->getHeight(m_position.x, m_position.z);
	if (m_position.y - y <= FLT_EPSILON) {
		m_position.y = y;
		m_velocity.y = 0;
	}
	GetTransform()->SetLocalPosition(m_position);
	
	Vec3 difPos = m_position - m_priorPosition;
	if (difPos.Length() > FLT_EPSILON)
	{
		GetGameObject()->GetCollider()->UpdateNodePos();
	}
}

void RigidBody::accelerate(Vec3 acc)
{
	m_acceleration += acc;
}

void RigidBody::addForce(Vec3 dir, FORCEMODE fm)
{
	switch (fm) {
	case FORCEMODE::FORCE:
		break;

	case FORCEMODE::ACCELERATION:
		accelerate(dir);
		break;

	case FORCEMODE::IMPULSE:
		accelerate(dir / m_mass);
		break;

	case FORCEMODE::VELOCITYCHANGE:
		m_velocity += dir;
		break;

	default:
		
		break;
	}
}

