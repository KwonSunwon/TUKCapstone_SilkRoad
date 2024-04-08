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
	void Move(Vec3 amount) { m_position += amount; }
	void MoveTo(Vec3 position) { m_position = position; }
	void AddForce(Vec3 amount) { m_force = amount; }


public:
	Vec3 GetPosition() { return m_position; }
	Vec3 GetLinearVelocity() { return m_linearVelocity; }
	float GetMass() { return m_mass; }
	float GetRestitution() { return m_restitution; }

	void SetLinearVelocity(Vec3 linearVelocity) { m_linearVelocity = linearVelocity; }

private:
	void MovementStep();

private:
	Vec3 m_position = {};
	Vec3 m_linearVelocity = {};
	Vec3 m_rotation = {};
	Vec3 m_rotationVelocity = {};
	Vec3 m_force = {};
	float m_mass = 50.f;					//Áú·®
	float m_restitution = 0.5f;
	bool m_isStatic = true;

	Vec3 m_gravity = { 0.f,980.f,0.f };

	
};

