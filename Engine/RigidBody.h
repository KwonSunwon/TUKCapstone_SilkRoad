#pragma once
#include "Component.h"

class Terrain;
class BaseCollider;
class Manifold;

class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void Awake() override;
	virtual void FinalUpdate() override;
	

public:
	//현재 위치에 대한 상대적인 좌표 이동
	void	Move(Vec3 amount);

	//현재 위치와 관계없는 절대적인 좌표 이동
	void	MoveTo(Vec3 position);

	//힘에 방향으로 힘을 가한다. 질량이 클수록 적은 영향을 받음
	void	AddForce(Vec3 amount, bool isPacketProcess = false);

	//프레임 워크에서 사용하는 함수이므로 임의사용X
	void	MovementStep(int iterations);


	//getter, setter
public:
	Vec3	GetPosition()		{ return m_position; }
	Vec3	GetLinearVelocity() { return m_linearVelocity; }
	Vec3	GetXZVelocity()		{ return Vec3(m_linearVelocity.x, 0, m_linearVelocity.z); }
	Vec3	GetRotation()		{ return m_rotation; }
	float	GetMass()			{ return m_mass; }
	float	GetRestitution()	{ return m_restitution; }
	
	float	GetInvMass()		{
								if (m_isStatic)return 0;
								else return m_invMass; }
	float	GetMaxSpeed()		{ return m_maxSpeed; }
	float	GetMaxAirSpeed()	{ return m_maxAirSpeed; }
	
	bool	GetStatic() { return m_isStatic; }
	bool	GetIsBlockBody() { return m_blockBody; }

	bool	GetIsFalling();

	//mass, restitution, static, maxSpeed는 설정해주지 않으면 디폴트 값으로 적용됨
	void	SetLinearVelocity(Vec3 linearVelocity) { m_linearVelocity = linearVelocity; }
	void	SetMass(float mass) { m_mass = mass; m_invMass = 1 / mass; }
	void	SetRestitution(float restitution) { m_restitution = restitution; }
	void	SetStatic(bool isStatic) { m_isStatic = isStatic; }
	void	SetMaxSpeed(float maxSpeed) { m_maxSpeed = maxSpeed; }
	void	SetfrictionCoef(float firctionCoef) { m_frictionCoef = firctionCoef; }
	void	SetUseGravity(bool useGravity) { m_useGravity = useGravity; }
	void	SetMaxAirSpeed(float maxAirSpeed) { m_maxAirSpeed = maxAirSpeed; }
	void	SetRotation(Vec3 rotation) { m_rotation = rotation; }
	void	SetBlock() { m_blockBody = true; }
	void	SetOverlap() { m_blockBody = false; }
	void	SetAutoFirction(bool autoFriction) { m_autoFriction = autoFriction; }

	shared_ptr<vector<shared_ptr<Manifold>>> GetCollideEvent() { return m_manifolds; }
	void	AddCollideEvent(shared_ptr<Manifold> event);
	
private:
	void ApplyFriction(int iterations);
	void ApplyAccVel(int iterations);

private:
	Vec3 m_position = {};					
	Vec3 m_linearVelocity = {};				
	Vec3 m_rotation = {};
	Vec3 m_rotationVelocity = {};
	Vec3 m_force = {};
	float m_mass = 50.f;			//kg단위				
	float m_restitution = 0.5f;		//탄성 0~1 0에 가까울수록 탄성이 줄어들음
	float m_invMass= 1/50.f;
	float m_inertia = 0.f;
	bool m_isStatic = false;
	float m_frictionCoef = 2.f;

	float m_maxSpeed = 1000.f;		//최고속도 cm/s단위 
	float m_maxAirSpeed = 1000.f;
	
	bool m_isLanded = true;

	Vec3 m_gravity = { 0.f,-980.f,0.f };
	bool m_useGravity = true;
	
	bool m_blockBody = true;
	bool m_autoFriction = true;
	shared_ptr<vector<shared_ptr<Manifold>>> m_manifolds;
	shared_ptr< Terrain > m_gameTerrain;
	shared_ptr<BaseCollider> m_baseCollider;

	
};

