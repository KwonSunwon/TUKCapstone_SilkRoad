#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	m_shadowCamera = make_shared<GameObject>();
	m_shadowCamera->AddComponent(make_shared<Transform>());
	m_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	m_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	m_lightInfo.position = GetTransform()->GetWorldPosition();

	m_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	m_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	m_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	m_shadowCamera->FinalUpdate();
}

void Light::Render()
{
	assert(m_lightIndex >= 0);

	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(m_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
		m_lightMaterial->SetTexture(3, shadowTex);

		for (int i = 0; i < SHADOWMAP_COUNT; ++i)
		{
			Matrix matVP = Camera::S_MatShadowView[i] * Camera::S_MatShadowProjection[i];
			m_lightMaterial->SetMatrix(i, matVP);
		}
	}
	else
	{
		float scale = 2 * m_lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	m_lightMaterial->SetInt(0, m_lightIndex);
	m_lightMaterial->PushGraphicsData();

	m_volumeMesh->Render();
}

void Light::RenderShadow()
{
	m_shadowCamera->GetCamera()->SortShadowObject();
	m_shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction)
{
	direction.Normalize();

	m_lightInfo.direction = direction;

	GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type)
{
	m_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		m_shadowCamera->GetCamera()->SetScale(1.f);
		m_shadowCamera->GetCamera()->SetFar(5000.f);
		m_shadowCamera->GetCamera()->SetWidth(5000);
		m_shadowCamera->GetCamera()->SetHeight(5000);
		m_shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);

		break;
	case LIGHT_TYPE::POINT_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		m_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		m_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}
}