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
		// 더 이상 나눌 수 없을 경우 반복문 종료
		if (currentNode->GetBB()->Extents.x / 2 < m_minSize)
			break;


		if (bs->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(bs);

			if (currentNode->GetBB()->Contains(*(sphereCollider->GetBoundingSphere()))==2) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*sphereCollider->GetBoundingSphere())==2)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// 완전히 포함되는 자식 노드가 없을 경우 반복문 종료
				if (notIncludedCount == 8)
					break;
			}
		}
		else if (bs->GetColliderType() == ColliderType::Box) {
			shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(bs);

			if (currentNode->GetBB()->Contains(*boxCollider->GetBoundingBox())==2) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxCollider->GetBoundingBox())==2)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// 완전히 포함되는 자식 노드가 없을 경우 반복문 종료
				if (notIncludedCount == 8)
					break;
			}
		}
		else if (bs->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(bs);

			if (currentNode->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == 2) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == 2)
					{
						currentNode = currentNode->GetChildNode(i);
						break;
					}

					notIncludedCount++;
				}

				// 완전히 포함되는 자식 노드가 없을 경우 반복문 종료
				if (notIncludedCount == 8)
					break;
			}
		}

	}

	// 오브젝트의 AABB가 속해야하는 위치에 저장
	currentNode->InsertObjectCollider(bs);
}

void OcTree::CollisionInspection(shared_ptr<BaseCollider> bs)
{
	// 루트 노드에서부터 파라미터로 넘어온 오브젝트 AABB가 속하는 노드를 찾음
	shared_ptr<OcNode> IncludedNode = FindColliderIncludedNode(bs, m_rootNode);

	

	// 부모 노드들에 대한 충돌 검사
	CollisionInspectionToParrent(bs, IncludedNode);
	// 자식 노드들에 대한 충돌 검사
	CollisionInspectionToChild(bs, IncludedNode);

}

void OcTree::Update()
{
	UpdateOcnode(m_rootNode);

}

void OcTree::UpdateOcnode(shared_ptr<OcNode> currentNode)
{
	currentNode->Update();
	for (int i = 0; i < currentNode->IncludedObjectAABBCount(); ++i)
	{
		if (currentNode->IncludedObjectAABB(i)->updatePos) {
			currentNode->IncludedObjectAABB(i)->updatePos = false;
			shared_ptr<BaseCollider> tempCol = currentNode->IncludedObjectAABB(i);
			int id =tempCol->GetColliderId();
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
	// 현재 노드에 파라미터로 넘어온 오브젝트 AABB가 있을 경우
	// 현재 노드를 반환
	if (bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(bs);

		if (currentNode->GetBB()->Contains(*sphereCollider->GetBoundingSphere())==2) {
			return currentNode;
		}
	}
	else if (bs->GetColliderType() == ColliderType::Box) {
		shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(bs);

		if (currentNode->GetBB()->Contains(*boxCollider->GetBoundingBox())==2) {
			return currentNode;
		}
	}
	else if (bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(bs);

		if (currentNode->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == 2) {
			return currentNode;
		}
	}
	


	shared_ptr<OcNode> IncludedNode = nullptr;

	// 현재 노드에 자식이 있을 경우 제일 첫 번째 자식부터 재귀로 검사
	if (currentNode->IsHaveChilds())
	{
		for (int i = 0; i < 8 && IncludedNode == nullptr; i++)
			IncludedNode = FindColliderIncludedNode(bs, currentNode->GetChildNode(i));
	}

	return IncludedNode;
}
float mult = 0;
float mult2 = 100;
void OcTree::CollisionInspectionToParrent(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	// 최상위 노드까지 왔을 경우 리턴
	if (currentNode == nullptr)
		return;


	for (int i = 0; i < currentNode->IncludedObjectAABBCount(); i++)
	{

		// 자기 자신에 대한 충돌검사는 수행하지 않음
		if (bs->GetColliderId() == currentNode->IncludedObjectAABB(i)->GetColliderId())
			continue;


		if (currentNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(currentNode->IncludedObjectAABB(i));
			if (bs->Intersects(sphereCollider->GetBoundingSphere())) {
				bs->setColor(Vec4(1, 0, 0, 0), true);


				shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
				shared_ptr<RigidBody> rb2 = sphereCollider->GetRigidBody();

				//rb1->SetUseGravity(false);

				float m1 = rb1->GetMass();
				float m2 = rb2->GetMass();

				// 각각의 강체의 초기 속도를 가져옵니다.
				Vec3 v1 = rb1->GetVelocity();
				Vec3 v2 = rb2->GetVelocity();

				// 운동량 보존을 이용하여 충돌 후의 속도를 계산합니다.
				// 완전탄성충돌에서는 두 물체가 충돌 후에 서로의 속도가 교환됩니다.
				Vec3 new_v1 = ((m1 - m2) / (m1 + m2)) * v1 + ((2 * m2) / (m1 + m2)) * v2;
				rb1->addForce(new_v1* mult, FORCEMODE::IMPULSE);

				Vec3 forceDir = rb1->GetPosition() - rb2->GetPosition();
				forceDir.Normalize();
				rb1->addForce(forceDir * rb1->GetMass() * mult2, FORCEMODE::IMPULSE);
			}
		}
		else if (currentNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::Box) {
			shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(currentNode->IncludedObjectAABB(i));
			if (bs->Intersects(boxCollider->GetBoundingBox())) {
				bs->setColor(Vec4(1, 0, 0, 0), true);
				
			}	
		}
		else if (currentNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(currentNode->IncludedObjectAABB(i));
			if (bs->Intersects(boxOrientedCollider->GetBoundingOrientedBox())) {
				bs->setColor(Vec4(1, 0, 0, 0), true);

				

				std::shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
				std::shared_ptr<RigidBody> rb2 = boxOrientedCollider->GetRigidBody();

				//rb1->SetUseGravity(false);

				float m1 = rb1->GetMass();
				float m2 = rb2->GetMass();

				// 각각의 강체의 초기 속도를 가져옵니다.
				Vec3 v1 = rb1->GetVelocity();
				Vec3 v2 = rb2->GetVelocity();

				// 운동량 보존을 이용하여 충돌 후의 속도를 계산합니다.
				// 완전탄성충돌에서는 두 물체가 충돌 후에 서로의 속도가 교환됩니다.
				Vec3 new_v1 = ((m1 - m2) / (m1 + m2)) * v1 + ((2 * m2) / (m1 + m2)) * v2;
				rb1->addForce(new_v1 * mult, FORCEMODE::IMPULSE);

				Vec3 forceDir = rb1->GetPosition() - rb2->GetPosition();
				forceDir.Normalize();
				rb1->addForce(forceDir * rb1->GetMass() * mult2, FORCEMODE::IMPULSE);
			}
		}
		
	}

	// 현재 노드의 부모에 대한 충돌 검사를 재귀적으로 수행
	CollisionInspectionToParrent(bs, currentNode->GetParentNode());
}

void OcTree::CollisionInspectionToChild(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	/*if (!currentNode->include) {
		currentNode->include = true;
		GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(currentNode->m_go);
	}*/
	// 자식이 없으면 리턴
	if (!currentNode->IsHaveChilds())
		return;

	

	for (int i = 0; i < 8; i++)
	{
		shared_ptr<OcNode> childNode = currentNode->GetChildNode(i);

		for (int i = 0; i < childNode->IncludedObjectAABBCount(); i++)
		{
			if (bs->GetColliderId() == childNode->IncludedObjectAABB(i)->GetColliderId())
				continue;
			// 파라미터로 넘어온 aabb와 자식 노드에 속하는 오브젝트 AABB와 충돌하는지 검사
			if (childNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(childNode->IncludedObjectAABB(i));
				if (bs->Intersects(sphereCollider->GetBoundingSphere())) {
					bs->setColor(Vec4(1, 0, 0, 0), true);


					std::shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
					std::shared_ptr<RigidBody> rb2 = sphereCollider->GetRigidBody();

					//rb1->SetUseGravity(false);

					float m1 = rb1->GetMass();
					float m2 = rb2->GetMass();

					// 각각의 강체의 초기 속도를 가져옵니다.
					Vec3 v1 = rb1->GetVelocity();
					Vec3 v2 = rb2->GetVelocity();

					// 운동량 보존을 이용하여 충돌 후의 속도를 계산합니다.
					// 완전탄성충돌에서는 두 물체가 충돌 후에 서로의 속도가 교환됩니다.
					Vec3 new_v1 = ((m1 - m2) / (m1 + m2)) * v1 + ((2 * m2) / (m1 + m2)) * v2;
					rb1->addForce(new_v1 * mult, FORCEMODE::IMPULSE);

					Vec3 forceDir = rb1->GetPosition() - rb2->GetPosition();
					forceDir.Normalize();
					rb1->addForce(forceDir * rb1->GetMass() * mult2, FORCEMODE::IMPULSE);
					
				}
			}
			else if (childNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::Box) {
				shared_ptr<BoxCollider> boxCollider = dynamic_pointer_cast<BoxCollider>(childNode->IncludedObjectAABB(i));
				if (bs->Intersects(boxCollider->GetBoundingBox())) {
					bs->setColor(Vec4(1, 0, 0, 0), true);
					
				}
			}
			else if (childNode->IncludedObjectAABB(i)->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<OrientedBoxCollider> boxOrientedCollider = dynamic_pointer_cast<OrientedBoxCollider>(childNode->IncludedObjectAABB(i));
				if (bs->Intersects(boxOrientedCollider->GetBoundingOrientedBox())) {
					bs->setColor(Vec4(1, 0, 0, 0), true);
					

					std::shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
					std::shared_ptr<RigidBody> rb2 = boxOrientedCollider->GetRigidBody();

					//rb1->SetUseGravity(false);

					float m1 = rb1->GetMass();
					float m2 = rb2->GetMass();

					// 각각의 강체의 초기 속도를 가져옵니다.
					Vec3 v1 = rb1->GetVelocity();
					Vec3 v2 = rb2->GetVelocity();

					// 운동량 보존을 이용하여 충돌 후의 속도를 계산합니다.
					// 완전탄성충돌에서는 두 물체가 충돌 후에 서로의 속도가 교환됩니다.
					Vec3 new_v1 = ((m1 - m2) / (m1 + m2)) * v1 + ((2 * m2) / (m1 + m2)) * v2;
					rb1->addForce(new_v1 * mult, FORCEMODE::IMPULSE);


					Vec3 forceDir = rb1->GetPosition() - rb2->GetPosition();
					forceDir.Normalize();
					rb1->addForce(forceDir * rb1->GetMass() * mult2, FORCEMODE::IMPULSE);
				}
			}
	
		}

		// 자식노드의 자식노드들에 대한 충돌 검사를 재귀적으로 수행
		CollisionInspectionToChild(bs, childNode);
	}
}
