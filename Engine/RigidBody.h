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


public:
	Vec3 GetPosition() { return m_position; }
	Vec3 GetLinearVelocity() { return m_linearVelocity; }


private:
	Vec3 m_position = {};
	Vec3 m_linearVelocity = {};
	Vec3 m_rotation = {};
	Vec3 m_angularVelocity = {};
	float m_mass = 1.f;					//Áú·®
	float m_restitution = 1.f;
	bool m_isStatic = true;

	bool m_isCheck = false;

	
};

