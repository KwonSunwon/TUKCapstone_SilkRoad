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
	float GetMass() { return m_mass; }
	void SetMass(float mass) { m_mass = mass; }

	float GetDrag() { return m_drag; }
	void SetDrag(float drag) { m_drag = drag; }

	float GetAngularDrag() { return m_angularDrag; }
	void SetAngularDrag(float angularDrag) { m_angularDrag = angularDrag; }

	bool GetUseGravity() { return m_useGravity; }
	void SetUseGravity(bool useGravity) { m_useGravity = useGravity; }

	bool GetIsKinematic() { return m_isKinematic; }
	void SetIsKinematic(bool isKinematic) { m_isKinematic = isKinematic; }

	bool GetIsLanding() { return m_isLanding; }

	float GetMaxAngularVelocity() { return m_maxAngularVelocity; }
	void SetMaxAngularVelocity(float maxAngularVelocity) { m_maxAngularVelocity = maxAngularVelocity; }

	float GetMaxVelocity() { return m_maxVelocity; }
	void SetMaxVelocity(float maxVelocity) { m_maxVelocity = maxVelocity; }

	float GetFrictionCoef() { return m_frictionCoef; }
	void SetFrictionCoef(float frictionCoef) { m_frictionCoef = frictionCoef; }

	Vec3 GetAngularVelocity() { return m_angularVelocity; }
	void SetAngularVelocity(const Vec3& angularVelocity) { m_angularVelocity = angularVelocity; }

	Vec3 GetVelocity() { return m_velocity; }
	void SetVelocity(const Vec3& velocity) { m_velocity = velocity; }

	Vec3 GetPosition() { return m_position; }
	void SetPosition(const Vec3& position) { m_position = position; }


private:
	float m_mass = 1.f;					//Áú·®
	float m_drag = 0.f;					//
	float m_angularDrag = 0.f;

	bool m_useGravity = true;
	bool m_isKinematic = false;
	bool m_isLanding = false;

	float m_maxAngularVelocity = 100.f;
	float m_maxVelocity = 1000.f;



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

