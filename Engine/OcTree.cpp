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
		// 더 이상 나눌 수 없을 경우 반복문 종료
		if (currentNode->GetBB()->Extents.x / 2 < m_minSize)
			break;


		if (bs->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<SphereCollider> sphereCollider = dynamic_pointer_cast<SphereCollider>(bs);

			if (currentNode->GetBB()->Contains(*(sphereCollider->GetBoundingSphere()))== DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*sphereCollider->GetBoundingSphere())== DirectX::CONTAINS)
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

			if (currentNode->GetBB()->Contains(*boxCollider->GetBoundingBox())== DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxCollider->GetBoundingBox())== DirectX::CONTAINS)
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

			if (currentNode->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == DirectX::CONTAINS) {
				if (!currentNode->IsHaveChilds())
					currentNode->SplitBy8();

				// 완전히 포함되지 않는 자식들의 개수
				int notIncludedCount = 0;

				for (int i = 0; i < 8; i++)
				{
					// 자식 노드에 완전히 포함되면 현재 노드를 
					// 자식 노드로 변경 후 처음단계부터 다시 검사
					if (currentNode->GetChildNode(i)->GetBB()->Contains(*boxOrientedCollider->GetBoundingOrientedBox()) == DirectX::CONTAINS)
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

	
	CollisionTerrain(bs, bs);
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
	// 현재 노드에 파라미터로 넘어온 오브젝트 AABB가 있을 경우
	// 현재 노드를 반환
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

	// 현재 노드에 자식이 있을 경우 제일 첫 번째 자식부터 재귀로 검사
	if (currentNode->IsHaveChilds())
	{
		for (int i = 0; i < 8 && IncludedNode == nullptr; i++)
			IncludedNode = FindColliderIncludedNode(bs, currentNode->GetChildNode(i));
	}

	return IncludedNode;
}


void OcTree::CollisionInspectionToParrent(shared_ptr<BaseCollider> bs, shared_ptr<OcNode> currentNode)
{
	// 최상위 노드까지 왔을 경우 리턴
	if (currentNode == nullptr)
		return;


	for (int i = 0; i < currentNode->IncludedObjectAABBCount(); i++)
	{
		shared_ptr<BaseCollider> bsDst = currentNode->IncludedObjectAABB(i);


		// 자기 자신에 대한 충돌검사는 수행하지 않음
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

		// 자기 자신에 대한 충돌검사는 수행하지 않음
		if (bs->GetColliderId() >= bsDst->GetColliderId())
			continue;

		// baseColliser가 Sphere인 경우
		if (bs->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();

			//대상이 Sphere인 경우
			if (bsDst->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

				if (!CollisionSphere(boundingSphereSrc, boundingSphereDst, normal, depth))
					continue;
			}

			//대상이 OBB인 경우
			else if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

				if (!CollisionSphereBox(boundingSphereSrc, boundingOrientedBoxDst, normal, depth,false))
					continue;
			}

		}

		// baseColliser가 OBB인 경우
		else if (bs->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

			//대상이 OBB인 경우
			if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

				if (!CollisionBox(boundingOrientedBoxSrc, boundingOrientedBoxDst, normal, depth))
					continue;
			}

			//대상이 Sphere인 경우
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

			// 자기 자신에 대한 충돌검사는 수행하지 않음
			if (bs->GetColliderId() >= bsDst->GetColliderId())
				continue;

			// baseColliser가 Sphere인 경우
			if (bs->GetColliderType() == ColliderType::Sphere) {
				shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();

				//대상이 Sphere인 경우
				if (bsDst->GetColliderType() == ColliderType::Sphere) {
					shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

					if (!CollisionSphere(boundingSphereSrc, boundingSphereDst, normal, depth))
						continue;
				}

				//대상이 OBB인 경우
				else if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
					shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();
					
					if(!CollisionSphereBox(boundingSphereSrc, boundingOrientedBoxDst, normal, depth, false))
						continue;
				}

			}

			// baseColliser가 OBB인 경우
			else if (bs->GetColliderType() == ColliderType::OrientedBox) {
				shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

				//대상이 OBB인 경우
				if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
					shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

					if (!CollisionBox(boundingOrientedBoxSrc, boundingOrientedBoxDst, normal, depth))
						continue;
				}

				//대상이 Sphere인 경우
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

		// 자식노드의 자식노드들에 대한 충돌 검사를 재귀적으로 수행
		CollisionInspectionToChild(bs, childNode);
	}
}

//터레인과의 충돌을 위한 함수
void OcTree::CollisionTerrain(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsTerrain)
{
	shared_ptr<RigidBody> rb = bs->GetRigidBody();
	if (rb->GetStatic())
		return;


	if (!m_terrain) {
		m_terrain = GET_SINGLE(SceneManager)->GetActiveScene()->m_terrain;
	}

	//터레인을 통해 현재 위치 기반 높이와 normal을 불러온다.
	Vec3 pos = bs->GetRigidBody()->GetPosition();
	shared_ptr<Vec3> norm = make_shared<Vec3>();
	shared_ptr<float> h = make_shared < float >();
	m_terrain->getHeight(pos.x, pos.z, h, norm);

	//정적메시와의 충돌을 가정하기 위해 임시의 육면체 충돌체를 만든다.
	shared_ptr<BoundingOrientedBox> OBB = make_shared<BoundingOrientedBox>();
	OBB->Center = Vec3(pos.x, *h-100, pos.z);
	OBB->Extents = Vec3 (100, 100, 100);


	shared_ptr<Vec3> normal = make_shared<Vec3>();
	shared_ptr<float> depth = make_shared<float>();

	*normal = { 0,0,0 };
	*depth = 0.f;

	//충돌 검사를 통해 겹쳐진 최소 거리와 방향을 얻는다
	if (bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();


		if (!CollisionSphereBox(boundingSphereSrc, OBB, normal, depth, false)){
			return;
		}

	}
	// baseColliser가 OBB인 경우


	else if (bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();

		//대상이 OBB인 경우

		if (!CollisionBox(boundingOrientedBoxSrc, OBB, normal, depth)){
			return;
		}
	}

	//겹쳐진 방향의 반대방향으로 움직인다.
	bs->setColor(Vec4(1, 0, 0, 0), true);
	rb->Move(-(*normal) * (*depth));

	//이후 터레인의 법선벡터 방향으로 힘을가한다.
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

