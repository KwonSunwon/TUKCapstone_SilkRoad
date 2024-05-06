#pragma once

class OcNode;
class GameObject;
class BaseCollider;

class Terrain;

class OcTree
{
private:
	shared_ptr<OcNode> m_rootNode;	// �ֻ��� ���
	int m_minSize;			// �ּ� ������

public:
	OcTree(int maxSize, int minSize);
	~OcTree();

	/* ������Ʈ�� AABB�� Ʈ���� ���� */
	void InsertObjectCollider(shared_ptr<BaseCollider> bs);
	/* �浹 �˻� */
	void CollisionInspection(shared_ptr<BaseCollider> bs);

	void CollisionTerrain(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsTerrain);

	void Update();
	void UpdateOcnode(shared_ptr<OcNode> currentNode);

	


private:
	/* ������Ʈ�� AABB�� ���ϴ� ��带 ã�� ��ȯ */
	shared_ptr<OcNode> FindColliderIncludedNode(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);
	/* ���� ��忡 �θ� ���� �浹 �˻縦 ���� */
	void CollisionInspectionToParrent(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);
	/* ���� ��忡 �ڽĿ� ���� �浹 �˻縦 ���� */
	void CollisionInspectionToChild(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode);

	

	shared_ptr<Terrain> m_terrain;
};