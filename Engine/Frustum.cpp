#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

void Frustum::FinalUpdate(Matrix matV, Matrix matP)
{
	/*Matrix matViewInv = Camera::S_MatView.Invert();
	Matrix matProjectionInv = Camera::S_MatProjection.Invert();*/
	Matrix matViewInv = matV.Invert();
	Matrix matProjectionInv = matP.Invert();
	Matrix matInv = matProjectionInv * matViewInv;

	vector<Vec3> worldPos =
	{
		::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),
		::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)
	};
	m_frustum = worldPos;

	m_planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
	m_planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
	m_planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
	m_planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
	m_planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
	m_planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
	for (const Vec4& plane : m_planes)
	{
		// n = (a, b, c)
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (normal.Dot(pos) + plane.w > radius)
			return false;
	}

	return true;
}

bool Frustum::ContainsSphereShadow(const Vec3& pos, float radius)
{
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		bool flag = true;
		FinalUpdate(Camera::S_MatShadowView[i], Camera::S_MatShadowProjection[i]);
		for (const Vec4& plane : m_planes)
		{
			// n = (a, b, c)
			Vec3 normal = Vec3(plane.x, plane.y, plane.z);

			// ax + by + cz + d > radius
			if (normal.Dot(pos) + plane.w > radius)
			{
				flag = false;
				break;
			}
		}
		if (flag == true)
			return true;
	}

	return false;
}