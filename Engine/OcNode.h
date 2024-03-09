#pragma once
#include "BaseCollider.h"

class BaseCollider;


class OcNode:public std::enable_shared_from_this<OcNode>
{
private: 
	shared_ptr<OcNode> m_parentNode;				// 부모 노드
	shared_ptr<OcNode> m_childNodes[8];					// 자식들의 노드
	shared_ptr<BoundingBox> m_bb;							// 이 노드의 AABB
	vector<shared_ptr<BaseCollider>> m_includedColliders;	// 이 노드에 속하는 오브젝트의 AABB
	
	int m_includedCollidersCount;				// 이 노드에 속하는 오브젝트의 AABB 개수
	bool m_isHaveChilds;						// 자식 노드가 존재하는지 여부

public:
	OcNode(shared_ptr<BoundingBox> bb, shared_ptr<OcNode> parentNode);
	~OcNode();

public:
	/* 오브젝트의 AABB를 등록 */
	void InsertObjectCollider(shared_ptr<BaseCollider> bs);
	/* 현재 노드의 AABB를 4분할 하여 자식 노드를 생성 */
	void SplitBy8();
	/* 현재 노드에 파라미터로 넘어온 오브젝트 AABB가 있는지 검사 */
	bool ObjectColliderContains(shared_ptr<BaseCollider> bs);

	void Update();
	void DeleteCol(int id);

	// Get, Set
public:
	shared_ptr<OcNode> GetParentNode() { return m_parentNode; };
	shared_ptr<OcNode> GetChildNode(int index) { return m_childNodes[index]; }
	shared_ptr<BoundingBox> GetBB() { return m_bb; }
	shared_ptr<BaseCollider> IncludedObjectAABB(int index) { return m_includedColliders[index]; }
	int IncludedObjectAABBCount() { return m_includedCollidersCount; }
	bool IsHaveChilds() { return m_isHaveChilds; }


	shared_ptr<GameObject> m_go;
	bool include = false;
};