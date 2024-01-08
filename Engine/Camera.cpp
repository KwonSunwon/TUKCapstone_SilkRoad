#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	m_width = static_cast<float>(GEngine->GetWindow().width);
	m_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	m_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	if (m_type == PROJECTION_TYPE::PERSPECTIVE)
		m_matProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	else
		m_matProjection = ::XMMatrixOrthographicLH(m_width * m_scale, m_height * m_scale, m_near, m_far);

	m_frustum.FinalUpdate();
}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	m_vecForward.clear();
	m_vecDeferred.clear();
	m_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (m_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				m_vecDeferred.push_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				m_vecForward.push_back(gameObject);
				break;
			}
		}
		else
		{
			m_vecParticle.push_back(gameObject);
		}
	}
}

void Camera::SortShadowObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	m_vecShadow.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		if (gameObject->IsStatic())
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (m_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		m_vecShadow.push_back(gameObject);
	}
}

void Camera::Render_Deferred()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	GET_SINGLE(InstancingManager)->Render(m_vecDeferred);
}

void Camera::Render_Forward()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	GET_SINGLE(InstancingManager)->Render(m_vecForward);

	for (auto& gameObject : m_vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}
}

void Camera::Render_Shadow()
{
	S_MatView = m_matView;
	S_MatProjection = m_matProjection;

	for (auto& gameObject : m_vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}
}