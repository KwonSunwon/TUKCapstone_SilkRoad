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

#include <chrono>

void NetworkPlayer::Awake()
{
	shared_ptr<RigidBody> rb = GetRigidBody();
	rb->SetStatic(false);
}

void NetworkPlayer::Update()
{
}

void NetworkPlayer::LateUpdate()
{
}

void NetworkPlayer::ProcessPacket(shared_ptr<PlayerPacket> packet)
{
	/*auto timeStamp = chrono::system_clock::now();
	string debugText = " Player " + to_string(packet->m_targetId) + " position: " + to_string(packet->m_position.x) + ", " + to_string(packet->m_position.y) + ", " + to_string(packet->m_position.z);
	debugText += " velocity: " + to_string(packet->m_velocity.x) + ", " + to_string(packet->m_velocity.y) + ", " + to_string(packet->m_velocity.z) + '\n';
	GET_SINGLE(NetworkManager)->m_log << timeStamp << debugText;*/

	shared_ptr<RigidBody> rb = GetRigidBody();
	shared_ptr<Transform> transform = GetTransform();
	rb->MoveTo(packet->m_position);
	rb->SetLinearVelocity(packet->m_velocity);
	auto rotation = Vec3(0, packet->m_rotation.y, 0);
	transform->SetLocalRotation(rotation);
	//rb->SetRotation(packet->m_rotation);
	if(GetAnimator()->GetCurrentClipIndex() != packet->m_animationIndex)
		GetAnimator()->Play(packet->m_animationIndex);
}

void NetworkPlayer::ChangeClass(shared_ptr<PlayerClassChangePacket> packet)
{
	int id = packet->m_classIndex;
	shared_ptr<MeshData> meshData;

	switch(id) {
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
