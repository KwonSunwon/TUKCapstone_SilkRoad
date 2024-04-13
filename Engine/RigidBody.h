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
class Terrain;
class BaseCollider;

class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void Awake() override;
	virtual void FinalUpdate() override;
	

public:
	void Move(Vec3 amount);
	void MoveTo(Vec3 position);
	void AddForce(Vec3 amount) { m_force = amount; }


public:
	Vec3 GetPosition() { return m_position; }
	Vec3 GetLinearVelocity() { return m_linearVelocity; }
	float GetMass() { return m_mass; }
	float GetRestitution() { return m_restitution; }
	bool GetIsStatic() { return m_isStatic; }
	float GetInvMass() {
		if (m_isStatic)return 0;
		else return m_invMass; }

	void SetMass(float mass) { m_mass = mass; m_invMass = 1 / mass; }
	void SetLinearVelocity(Vec3 linearVelocity) { m_linearVelocity = linearVelocity; }
	void SetStatic(bool isStatic) { m_isStatic = isStatic; }
	void MovementStep(int iterations);
private:
	

private:
	Vec3 m_position = {};
	Vec3 m_linearVelocity = {};
	Vec3 m_rotation = {};
	Vec3 m_rotationVelocity = {};
	Vec3 m_force = {};
	float m_mass = 50.f;					//Áú·®
	float m_restitution = 0.f;
	float m_invMass= 1/50.f;
	bool m_isStatic = false;

	float m_maxSpeed = 1000.f;
	

	Vec3 m_gravity = { 0.f,-980.f,0.f };
	

	shared_ptr< Terrain > m_gameTerrain;
	shared_ptr<BaseCollider> m_baseCollider;

	
};

