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
#include "Frustum.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;
Matrix Camera::S_MatShadowView[SHADOWMAP_COUNT];
Matrix Camera::S_MatShadowProjection[SHADOWMAP_COUNT];

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
	{
		m_matProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
		// 프러스텀 용 (Far 줄인 버전)
		Matrix matP = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far * 0.1f);
		m_frustum.FinalUpdate(m_matView, matP);
	}
	else
	{
		m_matProjection = ::XMMatrixOrthographicLH(m_width * m_scale, m_height * m_scale, m_near, m_far);
		m_frustum.FinalUpdate(m_matView, m_matProjection);
	}
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

		if (!gameObject->IsShadow())
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;
		// 그림자 렌더링은 반드시 프러스텀 컬링을 진행하나, Terrain은 무조건 그림자를 그림.
		if (!gameObject->GetTerrain())
		{
			if (m_frustum.ContainsSphereShadow(
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

	/*for (auto& gameObject : m_vecDeferred)
	{
		gameObject->GetMeshRenderer()->Render();
	}*/

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
	CalculateShadowMatrix();

	GET_SINGLE(InstancingManager)->Render(m_vecShadow, true);

	/*for (auto& gameObject : m_vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}*/
}

void Camera::CalculateShadowMatrix()
{
	constexpr array<float, SHADOWMAP_COUNT + 1> cascade = { 0.0f, 0.02f, 0.1f, 0.4f, 1.0f };
	vector<Vec3> frustum = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetFrustum().GetFrustum();
	if (frustum.size() != 8 ) {
		return;
	}

	for (int i = 0; i < cascade.size() - 1; ++i)
	{
		Vec3 wFrustum[8]{};
		for (int j = 0; j < 8; ++j)
		{
			wFrustum[j] = frustum[j];
		}
		for (int j = 0; j < 4; ++j)
		{
			Vec3 v{ wFrustum[j + 4] - wFrustum[j] };
			Vec3 n{ v * cascade[i] };
			Vec3 f{ v * cascade[i + 1] };

			wFrustum[j + 4] = wFrustum[j] + f;
			wFrustum[j] += n;
		}

		Vec3 center{};
		for (const auto& v : wFrustum)
		{
			center += v;
		}
		center /= 8;

		float radius = 0;
		for (const auto& v : wFrustum)
		{
			float dist = (v - center).Length();
			radius = max(radius, dist);
		}

		float offset{ max(5000.0f, radius) };
		Vec3 shadowLightPos{ center + GetTransform()->GetLook() * -offset };

		Matrix view = ::XMMatrixLookAtLH(shadowLightPos, center, Vec3(0, 1, 0));
		Matrix proj = ::XMMatrixOrthographicLH(radius * 2, radius * 2, 0.0f, 30000);

		Camera::S_MatShadowView[i] = view;
		Camera::S_MatShadowProjection[i] = proj;
		
	}
}