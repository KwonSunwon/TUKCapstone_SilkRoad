#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Timer.h"

class Material;
class StructuredBuffer;
class Mesh;

//mixamo 30������, with skin, uniform, fbx binary

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

	//�ִϸ��̼� �ε���, ���ϴ� �ð�, �� �� �����ϱ� ���ϴ� �Լ��� �Ѱ��ش�.
	//�� �ѹ� ������ �ش� �̺�Ʈ�� �����.
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

	shared_ptr<Material>			m_computeMaterial;
	shared_ptr<StructuredBuffer>	m_boneFinalMatrix;  // Ư�� �������� ���� ���
	bool							m_boneFinalUpdated = false;
};
