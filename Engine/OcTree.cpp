#include "pch.h"
#include "OcTree.h"
#include "OcNode.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OrientedBoxCollider.h"
#include"SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Terrain.h"
#include "Transform.h"
#include "Timer.h"
#include "Collision.h"

OcTree::OcTree(int maxSize, int minSize)
{

	m_rootNode = make_shared<OcNode>(make_shared<BoundingBox>(Vec3(16000, 0, 16000), Vec3(maxSize, maxSize, maxSize)), nullptr);
	m_minSize = minSize;
}

OcTree::~OcTree()
{

}

void OcTree::InsertObjectCollider(shared_ptr<BaseCollider> bs)
{
	shared_ptr<OcNode> currentNode = m_rootNode;
	while (true)
	{
		// �� �̻� ���� �� ���� ��� �ݺ��� ����
		if (currentNode->GetBB()->Extents.x / 2 < m_minSize)
			break;


		if (bs->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(bs);

			if (currentNode->GetBB()->Contains(*(sphereCollider->GetBoundingSphere()))== DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// ������ ���Ե��� �ʴ� �ڽĵ��� ����
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// �ڽ� ��忡 ������ ���ԵǸ� ���� ��带 
					// �ڽ� ���� ���� �� ó���ܰ���� �ٽ� �˻�
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*sphereCollider->GetBoundingSphere())== DirectX::CONTAINS)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// ������ ���ԵǴ� �ڽ� ��尡 ���� ��� �ݺ��� ����
				if (notIncludedCount == 8)
					break;
			}
		}
		else if (bs->GetColliderType() == ColliderType::Box) {
			shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(bs);

			if (currentNode->GetBB()->Contains(*boxCollider->GetBoundingBox())== DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// ������ ���Ե��� �ʴ� �ڽĵ��� ����
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// �ڽ� ��忡 ������ ���ԵǸ� ���� ��带 
					// �ڽ� ���� ���� �� ó���ܰ���� �ٽ� �˻�
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxCollider->GetBoundingBox())== DirectX::CONTAINS)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// ������ ���ԵǴ� �ڽ� ��尡 ���� ��� �ݺ��� ����
				if (notIncludedCount == 8)
					break;
			}
		}
		else if (bs->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(bs);

			if (currentNode->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// ������ ���Ե��� �ʴ� �ڽĵ��� ����
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// �ڽ� ��忡 ������ ���ԵǸ� ���� ��带 
					// �ڽ� ���� ���� �� ó���ܰ���� �ٽ� �˻�
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == DirectX::CONTAINS)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// ������ ���ԵǴ� �ڽ� ��尡 ���� ��� �ݺ��� ����
				if (notIncludedCount == 8)
					break;
			}
		}

	}

	// ������Ʈ�� AABB�� ���ؾ��ϴ� ��ġ�� ����
	currentNode->InsertObjectCollider(bs);
}

void OcTree::CollisionInspection(shared_ptr<BaseCollider> bs)
{
	// ��Ʈ ��忡������ �Ķ���ͷ� �Ѿ�� ������Ʈ AABB�� ���ϴ� ��带 ã��
	shared_ptr<OcNode> IncludedNode = FindColliderIncludedNode(bs, m_rootNode);

	
	CollisionTerrain(bs, bs);
	// �θ� ���鿡 ���� �浹 �˻�
	CollisionInspectionToParrent(bs, IncludedNode);
	// �ڽ� ���鿡 ���� �浹 �˻�
	CollisionInspectionToChild(bs, IncludedNode);

	
}



void OcTree::Update()
{
	//UpdateOcnode(m_rootNode);

}

void OcTree::UpdateOcnode(shared_ptr<OcNode> currentNode)
{
	
	currentNode->Update();
	for (int i = 0; i < currentNode->IncludedObjectAABBCount(); ++i)
	{
		if (currentNode->IncludedObjectAABB(i)->updatePos) {
			currentNode->IncludedObjectAABB(i)->updatePos = false;
			shared_ptr<BaseCollider> tempCol = currentNode->IncludedObjectAABB(i);
			int id = tempCol->GetColliderId();
			currentNode->DeleteCol(id);
			InsertObjectCollider(tempCol);
		}
	}

	if (!currentNode->IsHaveChilds())
		return;

	for (int i = 0; i < 8; i++)
	{
		shared_ptr<OcNode> childNode = currentNode->GetChildNode(i);
		UpdateOcnode(childNode);
	}

}



shared_ptr<OcNode> OcTree::FindColliderIncludedNode(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	// ���� ��忡 �Ķ���ͷ� �Ѿ�� ������Ʈ AABB�� ���� ���
	// ���� ��带 ��ȯ
	if (bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(bs);

		if (currentNode->GetBB()->Contains(*sphereCollider->GetBoundingSphere())==DirectX::CONTAINS) {
			return currentNode;
		}
	}
	else if (bs->GetColliderType() == ColliderType::Box) {
		shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(bs);

		if (currentNode->GetBB()->Contains(*boxCollider->GetBoundingBox())== DirectX::CONTAINS) {
			return currentNode;
		}
	}
	else if (bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(bs);

		if (currentNode->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == DirectX::CONTAINS) {
			return currentNode;
		}
	}
	


	shared_ptr<OcNode> IncludedNode = nullptr;

	// ���� ��忡 �ڽ��� ���� ��� ���� ù ��° �ڽĺ��� ��ͷ� �˻�
	if (currentNode->IsHaveChilds())
	{
		for (int i = 0; i < 8 && IncludedNode == nullptr; i++)
			IncludedNode = FindColliderIncludedNode(bs, currentNode->GetChildNode(i));
	}

	return IncludedNode;
}


void OcTree::CollisionInspectionToParrent(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	// �ֻ��� ������ ���� ��� ����
	if (currentNode == nullptr)
		return;


	for (int i = 0; i < currentNode->IncludedObjectAABBCount(); i++)
	{
		shared_ptr<BaseCollider> bsDst = currentNode->IncludedObjectAABB(i);


		// �ڱ� �ڽſ� ���� �浹�˻�� �������� ����
		if (bs->GetColliderId() >= bsDst->GetColliderId())
			continue;

		shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
		shared_ptr<RigidBody> rb2 = bsDst->GetRigidBody();
		if (rb1->GetStatic() && rb2->GetStatic())
			continue;

		shared_ptr<Vec3> normal = make_shared<Vec3>();
		shared_ptr<float> depth = make_shared<float>();

		*normal = { 0,0,0 };
		*depth = 0.f;

		// �ڱ� �ڽſ� ���� �浹�˻�� �������� ����
		if (bs->GetColliderId() >= bsDst->GetColliderId())
			continue;

		// baseColliser�� Sphere�� ���
		if (bs->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();

			//����� Sphere�� ���
			if (bsDst->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

				if (!CollisionSphere(boundingSphereSrc, boundingSphereDst, normal, depth))
					continue;
			}

			//����� OBB�� ���
			else if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

				if (!CollisionSphereBox(boundingSphereSrc, boundingOrientedBoxDst, normal, depth,false))
					continue;
			}

		}

		// baseColliser�� OBB�� ���
		else if (bs->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

			//����� OBB�� ���
			if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

				if (!CollisionBox(boundingOrientedBoxSrc, boundingOrientedBoxDst, normal, depth))
					continue;
			}

			//����� Sphere�� ���
			else if (bsDst->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

				if (!CollisionSphereBox(boundingSphereDst, boundingOrientedBoxSrc, normal, depth,true))
					continue;
			}
		}

		
		bs->setColor(Vec4(1, 0, 0, 0), true);
		bsDst->setColor(Vec4(1, 0, 0, 0), true);

		if (rb1->GetStatic()) {
			rb2->Move(*normal * (*depth));
		}

		else if (rb2->GetStatic()) {
			rb1->Move(-(*normal) * (*depth));
		}
		else {
			rb1->Move(-(*normal) * (*depth) / 2);
			rb2->Move(*normal * (*depth) / 2);
		}



		Vec3 relativeVelocity = rb2->GetLinearVelocity() - rb1->GetLinearVelocity();
		if (relativeVelocity.Dot(*normal) > 0.f) {
			continue;
		}

		float e = min(rb1->GetRestitution(), rb2->GetRestitution());
		float j = -(1.f + e) * relativeVelocity.Dot(*normal);
		j /= rb1->GetInvMass() + rb2->GetInvMass();
		Vec3 impulse = (*normal) * j;

		rb1->SetLinearVelocity(rb1->GetLinearVelocity() - impulse * rb1->GetInvMass());
		rb2->SetLinearVelocity(rb2->GetLinearVelocity() + impulse * rb2->GetInvMass());

	}

	// ���� ����� �θ� ���� �浹 �˻縦 ��������� ����
	CollisionInspectionToParrent(bs, currentNode->GetParentNode());
}

void OcTree::CollisionInspectionToChild(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	/*if (!currentNode->include) {
		currentNode->include = true;
		GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(currentNode->m_go);
	}*/
	// �ڽ��� ������ ����
	if (!currentNode->IsHaveChilds())
		return;

	

	for (int i = 0; i < 8; i++)
	{
		shared_ptr<OcNode> childNode = currentNode->GetChildNode(i);

		for (int m = 0; m < childNode->IncludedObjectAABBCount(); m++)
		{
			shared_ptr<BaseCollider> bsDst = childNode->IncludedObjectAABB(m);

			shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
			shared_ptr<RigidBody> rb2 = bsDst->GetRigidBody();
			if (rb1->GetStatic() && rb2->GetStatic())
				continue;


			
			shared_ptr<Vec3> normal = make_shared<Vec3>();
			shared_ptr<float> depth = make_shared<float>();

			*normal = { 0,0,0 };
			*depth = 0.f;

			// �ڱ� �ڽſ� ���� �浹�˻�� �������� ����
			if (bs->GetColliderId() >= bsDst->GetColliderId())
				continue;

			// baseColliser�� Sphere�� ���
			if (bs->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();

				//����� Sphere�� ���
				if (bsDst->GetColliderType() == ColliderType::Sphere) {
					shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

					if (!CollisionSphere(boundingSphereSrc, boundingSphereDst, normal, depth))
						continue;
				}

				//����� OBB�� ���
				else if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
					shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();
					
					if(!CollisionSphereBox(boundingSphereSrc, boundingOrientedBoxDst, normal, depth, false))
						continue;
				}

			}

			// baseColliser�� OBB�� ���
			else if (bs->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

				//����� OBB�� ���
				if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
					shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

					if (!CollisionBox(boundingOrientedBoxSrc, boundingOrientedBoxDst, normal, depth))
						continue;
				}

				//����� Sphere�� ���
				else if (bsDst->GetColliderType() == ColliderType::Sphere) {
					shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

					if (!CollisionSphereBox(boundingSphereDst, boundingOrientedBoxSrc, normal, depth, true))
						continue;
				}
			}	

			
			bs->setColor(Vec4(1, 0, 0, 0), true);
			bsDst->setColor(Vec4(1, 0, 0, 0), true);

			if (rb1->GetStatic()) {
				rb2->Move(*normal * (*depth));
			}
			
			else if (rb2->GetStatic()) {
				rb1->Move(-(*normal) * (*depth));
			}
			else {
				rb1->Move(-(*normal) * (*depth) / 2.f);
				rb2->Move(*normal * (*depth) / 2.f);
			}
			


			Vec3 relativeVelocity = rb2->GetLinearVelocity() - rb1->GetLinearVelocity();
			if (relativeVelocity.Dot(*normal) > 0.f) {
				continue;
			}

			float e = min(rb1->GetRestitution(), rb2->GetRestitution());
			float j = -(1.f + e) * relativeVelocity.Dot(*normal);
			j /= rb1->GetInvMass() + rb2->GetInvMass();
			Vec3 impulse = (*normal) * j;

			rb1->SetLinearVelocity(rb1->GetLinearVelocity() - impulse * rb1->GetInvMass());
			rb2->SetLinearVelocity(rb2->GetLinearVelocity() + impulse * rb2->GetInvMass());
		}

		// �ڽĳ���� �ڽĳ��鿡 ���� �浹 �˻縦 ��������� ����
		CollisionInspectionToChild(bs, childNode);
	}
}

//�ͷ��ΰ��� �浹�� ���� �Լ�
void OcTree::CollisionTerrain(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsTerrain)
{
	shared_ptr<RigidBody> rb = bs->GetRigidBody();
	if (rb->GetStatic())
		return;


	if (!m_terrain) {
		m_terrain = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain;
	}

	//�ͷ����� ���� ���� ��ġ ��� ���̿� normal�� �ҷ��´�.
	Vec3 pos = bs->GetRigidBody()->GetPosition();
	shared_ptr<Vec3> norm = make_shared<Vec3>();
	shared_ptr<float> h = make_shared < float >();
	m_terrain->getHeight(pos.x, pos.z, h, norm);

	//�����޽ÿ��� �浹�� �����ϱ� ���� �ӽ��� ����ü �浹ü�� �����.
	shared_ptr<BoundingOrientedBox> OBB = make_shared<BoundingOrientedBox>();
	OBB->Center = Vec3(pos.x, *h-100, pos.z);
	OBB->Extents = Vec3 (100, 100, 100);


	shared_ptr<Vec3> normal = make_shared<Vec3>();
	shared_ptr<float> depth = make_shared<float>();

	*normal = { 0,0,0 };
	*depth = 0.f;

	//�浹 �˻縦 ���� ������ �ּ� �Ÿ��� ������ ��´�
	if (bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();


		if (!CollisionSphereBox(boundingSphereSrc, OBB, normal, depth, false)){
			return;
		}

	}
	// baseColliser�� OBB�� ���


	else if (bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

		//����� OBB�� ���

		if (!CollisionBox(boundingOrientedBoxSrc, OBB, normal, depth)){
			return;
		}
	}

	//������ ������ �ݴ�������� �����δ�.
	bs->setColor(Vec4(1, 0, 0, 0), true);
	rb->Move(-(*normal) * (*depth));

	//���� �ͷ����� �������� �������� �������Ѵ�.
	Vec3 relativeVelocity = - rb->GetLinearVelocity();
	if (relativeVelocity.Dot(*normal) > 0.f) {
		return;
	}

	float e = min(rb->GetRestitution(),0.5);
	float j = -(1.f + e) * relativeVelocity.Dot(*norm);
	j /= rb->GetInvMass() + 1.f/1000.f;
	Vec3 impulse = (*norm) * j;

	rb->SetLinearVelocity(rb->GetLinearVelocity() - impulse * rb->GetInvMass());
	

}

