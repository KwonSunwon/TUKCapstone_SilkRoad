#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Resources.h"
#include "Animator.h"
#include "Engine.h"
#include "Camera.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::SetMaterial(shared_ptr<Material> material, uint32 idx)
{
	if (m_materials.size() <= static_cast<size_t>(idx))
		m_materials.resize(static_cast<size_t>(idx + 1));

	m_materials[idx] = material;
}

void MeshRenderer::Render()
{
	for (uint32 i = 0; i < m_materials.size(); i++)
	{
		shared_ptr<Material>& material = m_materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		GetTransform()->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}

		material->PushGraphicsData();
		m_mesh->Render(1, i);
	}
}

void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)
{
	for (uint32 i = 0; i < m_materials.size(); i++)
	{
		shared_ptr<Material>& material = m_materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		buffer->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}

		material->PushGraphicsData();
		m_mesh->Render(buffer, i);
	}
}

void MeshRenderer::RenderShadow()
{
	auto shadowMaterial = GET_SINGLE(Resources)->Get<Material>(L"Shadow");
	shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
	GetTransform()->PushData();
	shadowMaterial->SetInt(0, 0);
	if (GetAnimator())
	{
		GetAnimator()->PushData();
		shadowMaterial->SetInt(1, 1);
	}
	else
	{
		shadowMaterial->SetInt(1, 0);
	}
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		Matrix matVP = Camera::S_MatShadowView[i] * Camera::S_MatShadowProjection[i];
		shadowMaterial->SetMatrix(i, matVP);
	}
	shadowMaterial->SetTexture(3, shadowTex);
	shadowMaterial->PushGraphicsData();
	m_mesh->Render();
}

void MeshRenderer::RenderShadow(shared_ptr<InstancingBuffer>& buffer)
{
	auto shadowMaterial = GET_SINGLE(Resources)->Get<Material>(L"Shadow");
	shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
	buffer->PushData();
	shadowMaterial->SetInt(0, 1);
	if (GetAnimator())
	{
		GetAnimator()->PushData();
		shadowMaterial->SetInt(1, 1);
	}
	else
	{
		shadowMaterial->SetInt(1, 0);
	}
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		Matrix matVP = Camera::S_MatShadowView[i] * Camera::S_MatShadowProjection[i];
		shadowMaterial->SetMatrix(i, matVP);
	}
	shadowMaterial->SetTexture(3, shadowTex);
	shadowMaterial->PushGraphicsData();
	m_mesh->Render(buffer);
}

uint64 MeshRenderer::GetInstanceID()
{
	if (m_mesh == nullptr || m_materials.empty())
		return 0;

	//uint64 id = (_mesh->GetID() << 32) | _material->GetID();
	InstanceID instanceID{ m_mesh->GetID(), m_materials[0]->GetID() };
	return instanceID.id;
}