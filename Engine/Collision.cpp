#include "pch.h"
#include "Collision.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "OrientedBoxCollider.h"
#include "Transform.h"
array<int, 6> planeX{ 0,3,1,4,0,4 };
array<int, 6> planeY{ 1,2,5,5,4,0 };
array<int, 6> planeZ{ 2,6,6,6,5,3 };

array<int, 3> lineX{ 0,1,1 };
array<int, 3> lineY{ 1,2,5 };


bool CollisionSphere(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingSphere> subSphere, shared_ptr<Vec3> normal, shared_ptr<float> depth)
{

	Vec3 CenterSrc = mainSphere->Center;
	Vec3 CenterDst = subSphere->Center;

	float distance = SimpleMath::Vector3::Distance(CenterSrc, CenterDst);
	float radii = mainSphere->Radius + subSphere->Radius;

	if (distance >= radii)
		return false;

	*normal = CenterDst - CenterSrc;
	(*normal).Normalize();
	*depth = radii - distance;

	return true;
}

bool CollisionSphereBox(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<Vec3> normal, shared_ptr<float> depth, bool isBoxMain)
{
	shared_ptr<BoundingSphere> tempBS = make_shared<BoundingSphere>();
	tempBS->Center = mainCube->Center;
	tempBS->Radius = Vec3(mainCube->Extents).Length();
	if (!CollisionSphere(mainSphere, tempBS, normal, depth))
		return false;

	*depth = FLT_MAX;
	Vec3 axis;
	float axisDepth = 0.f;
	std::shared_ptr<float> minA = std::make_shared<float>();
	std::shared_ptr<float> maxA = std::make_shared<float>();
	std::shared_ptr<float> minB = std::make_shared<float>();
	std::shared_ptr<float> maxB = std::make_shared<float>();

	XMFLOAT3 cornersA[8];
	shared_ptr<array<Vec3, 8>> cornersArrayA = make_shared<array<Vec3, 8>>();
	mainCube->GetCorners(cornersA);

	for (int i = 0; i < 8; ++i) {
		(*cornersArrayA)[i] = cornersA[i];
	}

	float minDistance = FLT_MAX;
	float sphereRadius = mainSphere->Radius;
	Vec3 sphereCenter = mainSphere->Center;
	Vec3 boxCenter = mainCube->Center;
	Vec4 cubeOrientation = mainCube->Orientation;
	shared_ptr<XMVECTOR> axisRad = make_shared<XMVECTOR>();
	shared_ptr<float> angle = make_shared<float>();
	XMVECTOR ori { cubeOrientation.x, cubeOrientation.y, cubeOrientation.z, cubeOrientation.w };

	XMQuaternionToAxisAngle(axisRad.get(), angle.get(), ori);
	shared_ptr<Transform> transform = make_shared<Transform>();
	transform->SetLocalPosition(boxCenter);
	transform->SetLocalRotation(Vec3(*axisRad));
	transform->FinalUpdate();

	Matrix InvMat = transform->GetLocalToWorldMatrix().Invert();
	XMVECTOR transformSphereCenter = XMVector3TransformCoord(XMVECTOR{ sphereCenter.x, sphereCenter.y, sphereCenter.z }, InvMat);
	Vec3 inCubeSpaceSphereCenter = Vec3(XMVectorGetX(transformSphereCenter), XMVectorGetY(transformSphereCenter), XMVectorGetZ(transformSphereCenter));
	Vec3 closest;
	closest.x = max(-mainCube->Extents.x, min(inCubeSpaceSphereCenter.x, mainCube->Extents.x));
	closest.y = max(-mainCube->Extents.y, min(inCubeSpaceSphereCenter.y, mainCube->Extents.y));
	closest.z = max(-mainCube->Extents.z, min(inCubeSpaceSphereCenter.z, mainCube->Extents.z));

	if ((closest - inCubeSpaceSphereCenter).LengthSquared() < mainSphere->Radius * mainSphere->Radius) {
		Vec3 contactPoint = XMVector3TransformCoord(closest, transform->GetLocalToWorldMatrix());
		axis = sphereCenter - contactPoint;
		axis.Normalize();
		ProjectCube(cornersArrayA, axis, minA, maxA);
		ProjectSphere(mainSphere->Center, mainSphere->Radius, axis, minB, maxB);
		axisDepth = min(*maxB - *minA, *maxA - *minB);

		if (axisDepth < *depth) {
			*depth = axisDepth;
			*normal = axis;
		}
	}
	else {
		return false;
	}


	Vec3 dir = boxCenter - sphereCenter;
	if (isBoxMain)
		dir = sphereCenter - mainCube->Center;


	if (dir.Dot(*normal) < 0.f) {
		*normal = -(*normal);
	}
	return true;
}

bool CollisionBox(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingOrientedBox> subCube, shared_ptr<Vec3> normal, shared_ptr<float> depth)
{
	shared_ptr<BoundingSphere> tempBS1 = make_shared<BoundingSphere>();
	tempBS1->Center = mainCube->Center;
	tempBS1->Radius = Vec3(mainCube->Extents).Length();

	shared_ptr<BoundingSphere> tempBS2 = make_shared<BoundingSphere>();
	tempBS2->Center = subCube->Center;
	tempBS2->Radius = Vec3(subCube->Extents).Length();

	if (!CollisionSphere(tempBS1, tempBS2, normal, depth))
		return false;

	*depth = FLT_MAX;
	XMFLOAT3 cornersA[8];
	XMFLOAT3 cornersB[8];
	mainCube->GetCorners(cornersA);
	subCube->GetCorners(cornersB);
	shared_ptr<array<Vec3, 8>> cornersArrayA = make_shared<array<Vec3, 8>>();
	shared_ptr<array<Vec3, 8>> cornersArrayB = make_shared<array<Vec3, 8>>();
	shared_ptr<float> minA = make_shared<float>();
	shared_ptr<float> maxA = make_shared<float>();
	shared_ptr<float> minB = make_shared<float>();
	shared_ptr<float> maxB = make_shared<float>();

	for (int i = 0; i < 8; ++i) {
		(*cornersArrayA)[i] = cornersA[i];
		(*cornersArrayB)[i] = cornersB[i];
	}



	for (int k = 0; k < 3; ++k) {
		Vec3 va = (*cornersArrayA)[planeX[k]];
		Vec3 vb = (*cornersArrayA)[planeY[k]];
		Vec3 vc = (*cornersArrayA)[planeZ[k]];

		Vec3 edgeA = va - vb;
		Vec3 edgeB = va - vc;
		Vec3 axis = XMVector3Cross(edgeA, edgeB);
		axis.Normalize();

		ProjectCube(cornersArrayA, axis, minA, maxA);
		ProjectCube(cornersArrayB, axis, minB, maxB);


		if (*minA >= *maxB || *minB >= *maxA) {
			return false;
		}

		float axisDepth = min((*maxB - *minA), (*maxA - *minB));
		if (axisDepth < *depth) {
			*depth = axisDepth;
			*normal = axis;
		}
	}

	for (int k = 0; k < 3; ++k) {
		Vec3 va = (*cornersArrayB)[planeX[k]];
		Vec3 vb = (*cornersArrayB)[planeY[k]];
		Vec3 vc = (*cornersArrayB)[planeZ[k]];

		Vec3 edgeA = va - vb;
		Vec3 edgeB = va - vc;
		Vec3 axis = XMVector3Cross(edgeA, edgeB);
		axis.Normalize();

		ProjectCube(cornersArrayA, axis, minA, maxA);
		ProjectCube(cornersArrayB, axis, minB, maxB);


		if (*minA >= *maxB || *minB >= *maxA) {
			return false;
		}

		float axisDepth = min((*maxB - *minA), (*maxA - *minB));
		if (axisDepth < *depth) {
			*depth = axisDepth;
			*normal = axis;
		}
	}

	for (int q = 0; q < 3; ++q) {
		Vec3 va1 = cornersA[lineX[q]];
		Vec3 va2 = cornersA[lineY[q]];
		Vec3 edgeA = va1 - va2;

		for (int w = 0; w < 3; ++w) {
			Vec3 vb1 = cornersB[lineX[w]];
			Vec3 vb2 = cornersB[lineY[w]];
			Vec3 edgeB = vb1 - vb2;
			Vec3 axis = XMVector3Cross(edgeA, edgeB);
			axis.Normalize();


			if (axis.Length() < FLT_EPSILON)
				continue;

			ProjectCube(cornersArrayA, axis, minA, maxA);
			ProjectCube(cornersArrayB, axis, minB, maxB);


			if (*minA >= *maxB || *minB >= *maxA) {
				return false;
			}

			float axisDepth = min((*maxB - *minA), (*maxA - *minB));
			if (axisDepth < *depth) {
				*depth = axisDepth;
				*normal = axis;
			}

		}
	}

	Vec3 centerA = mainCube->Center;
	Vec3 centerB = subCube->Center;
	Vec3 dir = centerB - centerA;

	if (dir.Dot(*normal) < 0.f) {
		*normal = -(*normal);
	}

	return true;

}

void ProjectCube(shared_ptr<array<Vec3, 8>> corners, Vec3 axis, shared_ptr<float> minimum, shared_ptr<float> maximun)
{
	*minimum = FLT_MAX;
	*maximun = -FLT_MAX;

	for (int j = 0; j < 8; ++j) {
		Vec3 VA = (*corners)[j];
		float projA = VA.Dot(axis);

		*minimum = min(*minimum, projA);
		*maximun = max(*maximun, projA);
	}

}

void ProjectSphere(Vec3 center, float radius, Vec3 axis, shared_ptr<float> minimum, shared_ptr<float> maximun)
{	
	Vec3 dirAndRadius = axis * radius;
	Vec3 p1 = center + dirAndRadius;
	Vec3 p2 = center - dirAndRadius;

	*minimum = p1.Dot(axis);
	*maximun = p2.Dot(axis);
	if (*minimum > *maximun) {
		float temp = *minimum;
		*minimum = *maximun;
		*maximun = temp;
	}
}

void FindContactPoint(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingSphere> subSphere, shared_ptr<vector<Vec3>> cp)
{
	Vec3 mainToSub = Vec3(subSphere->Center) - Vec3(mainSphere->Center);
	mainToSub.Normalize();
	(*cp).push_back(mainSphere->Center + mainToSub * mainSphere->Radius);
}

void FindContactPoint(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal)
{
	//(*cp).push_back(mainSphere->Center + *normal * mainSphere->Radius);
	(*cp)[0]=(mainSphere->Center + *normal * mainSphere->Radius);
}

void FindContactPoint(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingSphere> mainSphere, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal)
{

	//(*cp).push_back(mainSphere->Center - *normal * mainSphere->Radius);
	(*cp)[0] = (mainSphere->Center - *normal * mainSphere->Radius);
}

void FindContactPoint(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingOrientedBox> subCube, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal)
{
	//*depth = FLT_MAX;
	XMFLOAT3 cornersA[8];
	XMFLOAT3 cornersB[8];
	mainCube->GetCorners(cornersA);
	subCube->GetCorners(cornersB);
	shared_ptr<array<Vec3, 8>> cornersArrayA = make_shared<array<Vec3, 8>>();
	shared_ptr<array<Vec3, 8>> cornersArrayB = make_shared<array<Vec3, 8>>();
	
	for (int i = 0; i < 8; ++i) {
		(*cornersArrayA)[i] = cornersA[i];
		(*cornersArrayB)[i] = cornersB[i];
	}



	for (int k = 0; k < 8; ++k) {
		Vec3 pt = cornersA[k];
		Vec3 axis = *normal;
		pt += axis * FLT_EPSILON;
		if (subCube->Contains(pt)) {
			//(*cp).push_back(cornersA[k]);
			(*cp)[0]=(cornersA[k]);
		}
	
	}

	for (int k = 0; k < 8; ++k) {
		Vec3 pt = cornersB[k];
		Vec3 axis = -(*normal);
		pt += axis * FLT_EPSILON;
		if (mainCube->Contains(pt)) {
			//(*cp).push_back(cornersA[k]);
			(*cp)[0] = (cornersA[k]);
		}
	}
}


void FindContactPoints(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsDst, shared_ptr<vector<Vec3>> contacts, shared_ptr<int> contactCount, shared_ptr<Vec3> normal)
{
	contacts->resize(4);
	for (Vec3 cp : *contacts) {
		cp = { 0,0,0 };
	}
	*contactCount = 0;



	if (bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();


		if (bsDst->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();
			FindContactPoint(boundingSphereSrc, boundingSphereDst, contacts);
			*contactCount = 1;
			
		}


		else if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();
			FindContactPoint(boundingSphereSrc, boundingOrientedBoxDst, contacts, normal);
			*contactCount = 1;
		}

	}


	else if (bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();


		if (bsDst->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();
			FindContactPoint(boundingOrientedBoxSrc, boundingOrientedBoxDst, contacts, normal);
			*contactCount = contacts.get()->size();
		}


		else if (bsDst->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();
			FindContactPoint(boundingOrientedBoxSrc, boundingSphereDst, contacts, normal);
			*contactCount = 1;
			
		}
	}
}
