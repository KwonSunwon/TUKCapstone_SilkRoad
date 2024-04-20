#pragma once
bool CollisionSphere(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingSphere> subSphere,
	shared_ptr<Vec3> normal, shared_ptr<float>depth);

bool CollisionSphereBox(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingOrientedBox> mainCube,
	shared_ptr<Vec3> normal, shared_ptr<float>depth, bool isBoxMain);

bool CollisionBox(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingOrientedBox> subCube,
	shared_ptr<Vec3> normal, shared_ptr<float>depth);

void ProjectCube(shared_ptr < array<Vec3, 8>>, Vec3 axis, shared_ptr<float> min, shared_ptr<float> max);

void ProjectSphere(Vec3 center, float radius, Vec3 axis, shared_ptr<float> min, shared_ptr<float> max);

void FindContactPoint(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingSphere> subSphere, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal);

void FindContactPoint(shared_ptr<BoundingSphere> mainSphere, shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal);

void FindContactPoint(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingSphere> mainSphere, shared_ptr<vector<Vec3>>  cp, shared_ptr<Vec3> normal);

void FindContactPoint(shared_ptr<BoundingOrientedBox> mainCube, shared_ptr<BoundingOrientedBox> subCube, shared_ptr<vector<Vec3>> cp, shared_ptr<Vec3> normal);

void FindContactPoints(shared_ptr<class BaseCollider> bs, shared_ptr<class BaseCollider> bsDst,
	shared_ptr<vector<Vec3>> contacts, shared_ptr<int> contactCount, shared_ptr<Vec3> normal);






