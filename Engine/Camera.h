#pragma once
#include "Component.h"
#include "Frustum.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE, // 원근 투영
	ORTHOGRAPHIC, // 직교 투영
};

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;

	void SetProjectionType(PROJECTION_TYPE type) { m_type = type; }
	PROJECTION_TYPE GetProjectionType() { return m_type; }

	void SortGameObject();
	void SortShadowObject();

	void Render_Deferred();
	void Render_Forward();
	void Render_Shadow();

	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			m_cullingMask |= (1 << layer);
		else
			m_cullingMask &= ~(1 << layer);
	}

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
	void SetCullingMask(uint32 mask) { m_cullingMask = mask; }
	bool IsCulled(uint8 layer) { return (m_cullingMask & (1 << layer)) != 0; }

	void SetNear(float value) { m_near = value; }
	void SetFar(float value) { m_far = value; }
	void SetFOV(float value) { m_fov = value; }
	void SetScale(float value) { m_scale = value; }
	void SetWidth(float value) { m_width = value; }
	void SetHeight(float value) { m_height = value; }

	Matrix& GetViewMatrix() { return m_matView; }
	Matrix& GetProjectionMatrix() { return m_matProjection; }

private:
	PROJECTION_TYPE m_type = PROJECTION_TYPE::PERSPECTIVE;

	float m_near = 1.f;
	float m_far = 1000.f;
	float m_fov = XM_PI / 4.f;
	float m_scale = 1.f;
	float m_width = 0.f;
	float m_height = 0.f;

	Matrix m_matView = {};
	Matrix m_matProjection = {};

	Frustum m_frustum;
	uint32 m_cullingMask = 0;

private:
	vector<shared_ptr<GameObject>>	m_vecDeferred;
	vector<shared_ptr<GameObject>>	m_vecForward;
	vector<shared_ptr<GameObject>>	m_vecParticle;
	vector<shared_ptr<GameObject>>	m_vecShadow;

public:
	// TEMP
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

