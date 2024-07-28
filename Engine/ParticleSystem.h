#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;

enum ParticleType {
	EXPLOSION,
	HEAL,
	PARTICLE_LAUNCHER,
	PARTICLE_PORTAL,
	PARTICLE_BIRD,
	PARTICLE_THUNDER,
	PARTICLE_ITEM,
	PARTICLE_DUST,
	PARTICLE_BEAM,
	PARTICLE_GATE,
	PARTICLE_GATE_COMP,
	PARTICLE_TYPE_COUNT
};

struct ParticleInfo
{
	Vec3	worldPos;
	float	curTime;
	Vec3	worldDir;
	float	lifeTime;
	int32	alive;
	int32	padding[3];
};

struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem(ParticleType type);
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();

	void SetArgs();
public:
	virtual void Load(const wstring& path) override { }
	virtual void Save(const wstring& path) override { }

	shared_ptr<Material> GetMaterial() { return m_material; }

public:
	shared_ptr<StructuredBuffer>	m_particleBuffer;
	shared_ptr<StructuredBuffer>	m_computeSharedBuffer;
	uint32							m_maxParticle = 1;

	shared_ptr<Material>		m_computeMaterial;
	shared_ptr<Material>		m_material;
	shared_ptr<Mesh>			m_mesh;

	float				m_createInterval = 0.005f;
	float				m_accTime = 0.f;

	float				m_minLifeTime = 0.75f;
	float				m_maxLifeTime = 0.75f;
	float				m_minSpeed = 0;
	float				m_maxSpeed = 0;
	float				m_startScale = 70.f;
	float				m_endScale = 70.f;

	float				m_exposeTime = 0.f;
	float				m_makeTime = 0.f;

	float				m_activeLength = 10000.f;

	int					m_row = 6;
	int					m_column = 6;

	bool				m_singleType = false;

	ParticleType		m_particleType = EXPLOSION;

	bool m_isSpawn = false;
};
