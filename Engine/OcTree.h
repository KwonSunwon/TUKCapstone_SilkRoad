#pragma once

class OcNode;
class GameObject;
class BaseCollider;

class Terrain;

class OcTree
{
private:
	shared_ptr<OcNode> m_rootNode;	// 최상위 노드
	int m_minSize;			// 최소 사이즈

public:
	OcTree(int maxSize, int minSize);
	~OcTree();

	/* 오브젝트의 AABB를 트리에 삽입 */
	void InsertObjectCollider(shared_ptr<BaseCollider> bs);
	/* 충돌 검사 */
	void CollisionInspection(shared_ptr<BaseCollider> bs);

	void CollisionTerrain(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsTerrain);

	void Update();
	void UpdateOcnode(shared_ptr<OcNode> currentNode);

	


private:
	/* 오브젝트의 AABB가 속하는 노드를 찾아 반환 */
	shared_ptr<OcNode> FindColliderIncludedNode(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);
	/* 현재 노드에 부모에 대한 충돌 검사를 수행 */
	void CollisionInspectionToParrent(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);
	/* 현재 노드에 자식에 대한 충돌 검사를 수행 */
	void CollisionInspectionToChild(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);

	

	shared_ptr<Terrain> m_terrain;
};