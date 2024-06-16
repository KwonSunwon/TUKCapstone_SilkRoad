#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	m_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	m_boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	m_updateTime += DELTA_TIME;

	const AnimClipInfo& animClip = m_animClips->at(m_clipIndex);
	if (m_updateTime >= animClip.duration)
	{
		m_firstLapCompleted = true;
		m_updateTime = 0.f;
	}
	if (m_blend_animation) {
		m_blend_updateTime -= DELTA_TIME;
	}
	if (m_blend_updateTime < 0.f) {
		m_blend_animation = false;
	}

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	m_frame = static_cast<int32>(m_updateTime * ratio);
	m_frame = min(m_frame, animClip.frameCount - 1);
	m_nextFrame = min(m_frame + 1, animClip.frameCount - 1);
	m_frameRatio = static_cast<float>(m_frame - m_frame);

	ExecuteEventFunctions();
}

void Animator::ExecuteEventFunctions()
{
	for (auto it = m_eventFunction.begin(); it != m_eventFunction.end(); ) {
		auto& [animInfo, event] = *it;
		if (m_clipIndex == animInfo.first && abs(m_updateTime - animInfo.second) < 0.02) {
			event();
			it = m_eventFunction.erase(it);
		}
		else {
			++it;
		}
	}
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	m_animClips = animClips;
}

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(m_bones->size());
	if (m_boneFinalMatrix->GetElementCount() < boneCount)
		m_boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	// Compute Shader
	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(m_blend_clipIndex)->PushComputeSRVData(SRV_REGISTER::t6);
	mesh->GetBoneFrameDataBuffer(m_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	m_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	m_computeMaterial->SetInt(0, boneCount);
	m_computeMaterial->SetInt(1, m_frame);
	m_computeMaterial->SetInt(2, m_nextFrame);
	m_computeMaterial->SetInt(3, m_blend_frame);
	m_computeMaterial->SetFloat(0, m_frameRatio);
	m_computeMaterial->SetFloat(2, 1 - m_blend_updateTime / 0.25);

	if(m_blend_animation)
		m_computeMaterial->SetFloat(3, 1.f);
	else
		m_computeMaterial->SetFloat(3, -1.f);

	uint32 groupCount = (boneCount / 256) + 1;
	m_computeMaterial->Dispatch(groupCount, 1, 1);

	// Graphics Shader
	m_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx)
{
	assert(idx < m_animClips->size());
	m_blend_clipIndex = m_clipIndex;
	m_clipIndex = idx;
	m_firstLapCompleted = false;
	m_updateTime = 0.f;

	m_blend_updateTime = 0.25f;
	m_blend_animation = true;
	m_blend_frame = m_frame;
}

void Animator::SetEventFunction(int32 idx, float updateTime, function<void()> func)
{
	m_eventFunction.push_back(make_pair(make_pair(idx, updateTime), func));
}
