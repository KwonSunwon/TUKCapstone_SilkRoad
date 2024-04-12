#include "pch.h"
#include "OcNode.h"
#include "BaseCollider.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"

OcNode::OcNode(shared_ptr<BoundingBox> bb, shared_ptr<OcNode> parentNode)
{
	include = false;
	m_bb = bb;
	m_parentNode = parentNode;
	m_includedCollidersCount = 0;
	m_isHaveChilds = false;

	if (DEBUG_MODE)
	{
		m_go = make_shared<GameObject>();
		m_go->AddComponent(make_shared<Transform>());
		m_go->GetTransform()->SetLocalPosition(m_bb->Center);
		Vec3 extents = m_bb->Extents;
		m_go->GetTransform()->SetLocalScale(extents * 2);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);

			material->SetInt(3, 1);
			material->SetVec4(3, Vec4(1, 0, 0, 1));
			meshRenderer->SetMaterial(material);
		}
		m_go->AddComponent(meshRenderer);
	}
	
}

OcNode::~OcNode()
{

}

void OcNode::InsertObjectCollider(shared_ptr<BaseCollider> bs)
{
	// 현재 노드에 속하는 오브젝트 AABB로 삽입
	m_includedColliders.push_back(bs);
	m_includedCollidersCount++;
}

void OcNode::SplitBy8()
{
	// 절반 위치를 찾아서 4분할 되는 자식 노드들을 생성
	Vec3 center = m_bb->Center;
	float size = m_bb->Extents.x;
	m_childNodes[0] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(size / 2, size / 2, size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[1] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(size / 2, size / 2, -size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[2] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(size / 2, -size / 2, size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[3] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(size / 2, -size / 2, -size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[4] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(-size / 2, size / 2, size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[5] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(-size / 2, size / 2, -size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[6] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(-size / 2, -size / 2, size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());
	m_childNodes[7] = make_shared<OcNode>(make_shared<BoundingBox>(center + Vec3(-size / 2, -size / 2, -size / 2), Vec3(size / 2, size / 2, size / 2)), shared_from_this());


	m_isHaveChilds = true;
	
}

bool OcNode::ObjectColliderContains(shared_ptr<BaseCollider> bs)
{
	
	// 현재 노드에 파라미터로 넘어온 aabb가 존재하는지 판단
	for (int i = 0; i < m_includedCollidersCount; i++)
	{
		if (m_includedColliders[i]->GetColliderId() == bs->GetColliderId())
			return true;
	}

	return false;
}

void OcNode::Update()
{
	// 현재 노드에 파라미터로 넘어온 aabb가 존재하는지 판단
	
	if (DEBUG_MODE) {
		if (m_includedCollidersCount > 0) {
			if (!include) {
				include = true;
				GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_go);
			}
			m_go->GetMeshRenderer()->GetMaterial()->SetInt(3, true);
			m_go->GetMeshRenderer()->GetMaterial()->SetVec4(3, Vec4(0, 1, 0, 0));
		}

		if (m_includedCollidersCount == 0) {
			if (include) {
				include = false;
				GET_SINGLE(SceneManager)->GetActiveScene()->RemoveGameObject(m_go);
			}
			m_go->GetMeshRenderer()->GetMaterial()->SetInt(3, false);
		}
	}
}

void OcNode::DeleteCol(int id)
{
	int idx = 0;
	for (int i = 0; i < m_includedCollidersCount; i++)
	{
		if (m_includedColliders[i]->GetColliderId() == id) {
			m_includedColliders.erase(m_includedColliders.begin() + i);
			m_includedCollidersCount--;
		}
	}
	/*for (auto col : m_includedColliders)
	{
		if (col->GetColliderId()==id) {
			m_includedColliders.erase(m_includedColliders.begin() + idx);
			m_includedCollidersCount--;
		}
		idx++;
	}*/
}
