#include "pch.h"
#include "GameObject.h"
#include "NetworkPlayer.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Animator.h"
#include "Network.h"
#include "InteractiveObject.h"
#include "Resources.h"
#include "Mesh.h"
#include "MeshData.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "UpgradeManager.h"
#include <chrono>

void NetworkPlayer::Awake()
{
	SetMonovihaviourName("NetworkPlayer");

	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
}

void NetworkPlayer::Update()
{
}

void NetworkPlayer::LateUpdate()
{
	if(m_hp <= 0) {
		m_isActivated = false;
	}
}

void NetworkPlayer::ProcessPacket(shared_ptr<PlayerPacket> packet)
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	auto rotation = Vec3(0, packet->m_rotation.y, 0);
	transform->SetLocalRotation(rotation);
	//rb->SetRotation(packet->m_rotation);
	if(GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
	m_hp = packet->m_hp;
	m_maxHp = packet->m_maxHp;
}

void NetworkPlayer::ChangeClass(int classIndex)
{
	GET_SINGLE(UpgradeManager)->m_usedClass[m_classIdx - 5] = true;
	GET_SINGLE(UpgradeManager)->m_usedClass[classIndex - 5] = false;
	m_classIdx = classIndex;
	shared_ptr<MeshData> meshData;
	switch(m_classIdx) {
	case EnumInteract::CHARACTER_CHANGER1:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Dealer.fbx");
		break;

	case EnumInteract::CHARACTER_CHANGER2:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Healer.fbx");
		break;

	case EnumInteract::CHARACTER_CHANGER3:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Launcher.fbx");
		break;

	case EnumInteract::CHARACTER_CHANGER4:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Tanker.fbx");
		break;
	}

	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
	GetMeshRenderer()->SetMesh(gameObjects[0]->GetMeshRenderer()->GetMesh());
	GetGameObject()->AddComponent(gameObjects[0]->GetAnimator());
}
