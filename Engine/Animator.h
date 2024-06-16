#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Timer.h"

class Material;
class StructuredBuffer;
class Mesh;

//mixamo 30프레임, with skin, uniform, fbx binary

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

public:
	void SetBones(const vector<BoneInfo>* bones) { m_bones = bones; }
	void SetAnimClip(const vector<AnimClipInfo>* animClips);
	void PushData();

	float GetUpadteTime(){ return m_updateTime; }

	int32 GetAnimCount() { return static_cast<uint32>(m_animClips->size()); }
	int32 GetCurrentClipIndex() { return m_clipIndex; }

	bool IsFirstLapCompleted() { return m_firstLapCompleted; }
	bool IsAnimationEndOnThisFrame() { return m_updateTime + DELTA_TIME >= m_animClips->at(m_clipIndex).duration; }
	void Play(uint32 idx);

	//애니메이션 인덱스, 원하는 시간, 그 때 동작하길 원하는 함수를 넘겨준다.
	//딱 한번 실행후 해당 이벤트는 지운다.
	void SetEventFunction(int32 idx, float updateTime, function<void()> func);

public:
	virtual void FinalUpdate() override;

private:
	void ExecuteEventFunctions();

	const vector<BoneInfo>* m_bones;
	const vector<AnimClipInfo>* m_animClips;
	vector<pair<pair<int32, float >, function<void()>>> m_eventFunction;

	float							m_updateTime = 0.f;
	
	int32							m_clipIndex = 0;
	int32							m_frame = 0;
	int32							m_nextFrame = 0;
	float							m_frameRatio = 0;
	bool							m_firstLapCompleted = false;

	bool							m_blend_animation = false;
	float							m_blend_updateTime = 0.f;
	int32							m_blend_clipIndex = 0;
	int32							m_blend_frame = 0;

	shared_ptr<Material>			m_computeMaterial;
	shared_ptr<StructuredBuffer>	m_boneFinalMatrix;  // 특정 프레임의 최종 행렬
	bool							m_boneFinalUpdated = false;
};
