#include "pch.h"
#include "Collision.h"
array<int, 3> planeX{ 0,3,1 };
array<int, 3> planeY{ 1,2,5 };
array<int, 3> planeZ{ 2,6,6 };

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

	for (int k = 0; k < 3; ++k) {
		Vec3 va = (*cornersArrayA)[planeX[k]];
		Vec3 vb = (*cornersArrayA)[planeY[k]];
		Vec3 vc = (*cornersArrayA)[planeZ[k]];

		Vec3 edgeA = va - vb;
		Vec3 edgeB = va - vc;
		Vec3 axis = XMVector3Cross(edgeA, edgeB);
		axis.Normalize();

		ProjectCube(cornersArrayA, axis, minA, maxA);
		ProjectSphere(mainSphere->Center, mainSphere->Radius, axis, minB, maxB);

		if (*minA >= *maxB || *minB >= *maxA) {
			return false;
		}

		axisDepth = min(*maxB - *minA, *maxA - *minB);
		if (axisDepth < *depth) {
			*depth = axisDepth;
			*normal = axis;
		}
	}

	int result = -1;
	float minDistance = FLT_MAX;
	Vec3 sphereCenter = mainSphere->Center;
	Vec3 boxCenter = mainCube->Center;

	for (int j = 0; j < 8; ++j) {
		Vec3 v = cornersA[j];
		float distance = (v - sphereCenter).Length();
		if (distance < minDistance) {
			minDistance = distance;
			result = j;
		}
	}
	Vec3 cp = cornersA[result];
	axis = cp - sphereCenter;
	axis.Normalize();

	ProjectCube(cornersArrayA, axis, minA, maxA);
	ProjectSphere(mainSphere->Center, mainSphere->Radius, axis, minB, maxB);
	if (*minA >= *maxB || *minB >= *maxA)
	{
		return false;
	}
	axisDepth = min(*maxB - *minA, *maxA - *minB);
	if (axisDepth < *depth) {
		*depth = axisDepth;
		*normal = axis;
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

			*minA = FLT_MAX;
			*maxA = -FLT_MAX;

			*minB = FLT_MAX;
			*maxB = -FLT_MAX;

			for (int j = 0; j < 8; ++j) {
				Vec3 VA = cornersA[j];
				float projA = VA.Dot(axis);
				*minA = min(*minA, projA);
				*maxA = max(*maxA, projA);
			}

			for (int j = 0; j < 8; ++j) {
				Vec3 VB = cornersB[j];
				float projB = VB.Dot(axis);
				*minB = min(*minB, projB);
				*maxB = max(*maxB, projB);
			}

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

void ProjectSphere(Vec3 center, float radius, Vec3 axis, shared_ptr<float> min, shared_ptr<float> max)
{
	Vec3 dirAndRadius = axis * radius;
	Vec3 p1 = center + dirAndRadius;
	Vec3 p2 = center - dirAndRadius;

	*min = p1.Dot(axis);
	*max = p2.Dot(axis);
	if (*min > *max) {
		float temp = *min;
		*min = *max;
		*max = temp;
	}
}
