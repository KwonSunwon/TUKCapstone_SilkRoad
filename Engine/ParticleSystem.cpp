#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem(ParticleType type) : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	

	m_computeSharedBuffer = make_shared<StructuredBuffer>();
	m_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	m_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	


	switch (type)
	{
	case EXPLOSION:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle0");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle0");


		m_singleType = true;
		m_maxParticle = 1;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"Explosion", L"..\\Resources\\Texture\\Particle\\Explosion.png");
		m_material->SetTexture(0, tex);
		m_startScale = 100.f;
		m_endScale = 100.f;
		m_minSpeed = 0.f;
		m_maxSpeed = 0.f;
		m_minLifeTime = 0.75f;
		m_maxLifeTime = 0.75f;
		m_column = 4;
		m_row = 4;
		m_computeMaterial->SetInt(3, 0);
		m_exposeTime = 0.75f;
		
		break;
	}


	case HEAL:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle1");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle1");

		m_singleType = false;
		m_maxParticle = 50;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"Heal", L"..\\Resources\\Texture\\Particle\\Heal.png");
		m_material->SetTexture(0, tex);
		m_startScale = 100.f;
		m_endScale = 100.f;
		m_minSpeed = 100.f;
		m_maxSpeed = 200.f;
		m_minLifeTime = 0.75f;
		m_maxLifeTime = 0.75f;
		m_column = 4;
		m_row = 4;
		m_computeMaterial->SetInt(3, 1);
		m_exposeTime = 10.f;
		break;
	}

	case PARTICLE_LAUNCHER:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle2");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle2");

		m_singleType = true;
		m_maxParticle = 1;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"LauncherP", L"..\\Resources\\Texture\\Particle\\Launcher.png");
		m_material->SetTexture(0, tex);
		m_startScale = 750.f;
		m_endScale = 1500.f;
		m_minSpeed = 0.f;
		m_maxSpeed = 0.f;
		m_minLifeTime = 2.f;
		m_maxLifeTime = 2.f;
		m_column = 6;
		m_row = 6;
		m_computeMaterial->SetInt(3, 0);
		m_exposeTime = 2.f;
		break;
	}

	case PARTICLE_PORTAL:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle3");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle3");

		m_singleType = true;
		m_maxParticle = 1;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"PortalP", L"..\\Resources\\Texture\\Particle\\Portal.png");
		m_material->SetTexture(0, tex);
		m_startScale = 100.f;
		m_endScale = 100.f;
		m_minSpeed = 0.f;
		m_maxSpeed = 0.f;
		m_minLifeTime = 0.5f;
		m_maxLifeTime = 0.5f;
		m_column = 4;
		m_row = 4;

		m_computeMaterial->SetInt(3, 0);
		m_exposeTime = 5.f;
		break;
	}

	case EXPLOSION4:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle0");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle0");

		m_singleType = true;
		m_maxParticle = 1;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"Explosion4", L"..\\Resources\\Texture\\Particle\\Explosion4.png");
		m_material->SetTexture(0, tex);
		m_startScale = 100.f;
		m_endScale = 100.f;
		m_minSpeed = 0.f;
		m_maxSpeed = 0.f;
		m_minLifeTime = 0.75f;
		m_maxLifeTime = 0.75f;
		m_column = 5;
		m_row = 5;
		break;
	}

	case EXPLOSION5:
	{
		m_material = GET_SINGLE(Resources)->Get<Material>(L"Particle0");
		m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle0");

		m_singleType = true;
		m_maxParticle = 1;
		shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
			L"Explosion5", L"..\\Resources\\Texture\\Particle\\Explosion5.png");
		m_material->SetTexture(0, tex);
		m_startScale = 100.f;
		m_endScale = 100.f;
		m_minSpeed = 0.f;
		m_maxSpeed = 0.f;
		m_minLifeTime = 0.75f;
		m_maxLifeTime = 0.75f;
		m_column = 5;
		m_row = 5;
		break;
	}

	default:
		break;
	}

	m_particleType = type;
	m_particleBuffer = make_shared<StructuredBuffer>();
	m_particleBuffer->Init(sizeof(ParticleInfo), m_maxParticle);
	
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::FinalUpdate()
{
	if (!m_isSpawn)
		return;

	m_accTime += DELTA_TIME;
	m_makeTime += DELTA_TIME;

	if (m_accTime > m_exposeTime) {
		GetTransform()->SetLocalPosition(Vec3(0.f, -100000.f, 0.f));
		m_isSpawn = false;
	}

	int32 add = 0;

	if (m_singleType)
	{
		add = 1;
	}
	else if (!m_singleType) {
		if (m_createInterval < m_makeTime)
		{
			m_makeTime = m_makeTime - m_createInterval;
			add = 1;
		}
	}
	
	

	m_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	m_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	m_computeMaterial->SetInt(0, m_maxParticle);
	m_computeMaterial->SetInt(1, add);

	m_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, m_accTime));
	m_computeMaterial->SetVec4(0, Vec4(m_minLifeTime, m_maxLifeTime, m_minSpeed, m_maxSpeed));

	m_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	if (!m_isSpawn)
		return;

	GetTransform()->PushData();

	m_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	m_material->SetFloat(0, m_startScale);
	m_material->SetFloat(1, m_endScale);
	m_material->PushGraphicsData();
	m_material->SetInt(0, m_row);
	m_material->SetInt(1, m_column);

	m_mesh->Render(m_maxParticle);
}

void ParticleSystem::SetArgs()
{
	m_accTime = 0.f;
	m_isSpawn = true;
}
