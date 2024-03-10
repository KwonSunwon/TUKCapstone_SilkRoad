#pragma once
#include "Component.h"

enum class FORCEMODE : uint8
{
	FORCE,
	ACCELERATION,
	IMPULSE,
	VELOCITYCHANGE,
	END,
};

class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void Awake() override;
	virtual void FinalUpdate() override;
	

public:

	void addForce(Vec3, FORCEMODE);

public:
	float m_mass = 1.f;
	float m_drag = 0.f;
	float m_angularDrag = 0.f;

	bool m_useGravity = true;
	bool m_isKinematic = false;

	

	float m_maxAngularVelocity = 100.f;
	float m_maxVelocity = 100.f;

	

private:
	Vec3 m_gravity = { 0.0,-980,0.0 };
	float m_frictionCoef = 1.f;
	
	Vec3 m_position = {};
	Vec3 m_acceleration = {};
	Vec3 m_rotation = {};
	Vec3 m_angularVelocity = {};
	Vec3 m_velocity = {};
	Vec3 m_priorPosition = {};



	void applyGravity();
	void applyFriction();
	void applyDrag();
	void updatePosition();
	void accelerate(Vec3);
	
};

