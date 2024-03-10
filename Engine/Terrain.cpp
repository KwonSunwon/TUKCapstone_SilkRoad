#include "pch.h"
#include "Terrain.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"


Terrain::Terrain() : Component(COMPONENT_TYPE::TERRAIN)
{

}

Terrain::~Terrain()
{
}

void Terrain::Init(int32 sizeX, int32 sizeZ)
{
	m_sizeX = sizeX;
	m_sizeZ = sizeZ;

	m_material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");

	m_material->SetInt(1, m_sizeX);
	m_material->SetInt(2, m_sizeZ);
	m_material->SetFloat(0, m_maxTesselation);

	shared_ptr<Texture> heightMap = GET_SINGLE(Resources)->Load<Texture>(L"HeightMap", L"..\\Resources\\Texture\\Terrain\\height.png");
	Vec2 v = Vec2(heightMap->GetWidth(), heightMap->GetHeight());
	m_texSizeX = v.x;
	m_texSizeZ = v.y;
	m_material->SetVec2(0, Vec2(heightMap->GetWidth(), heightMap->GetHeight()));
	m_material->SetVec2(1, Vec2(1000.f, 5000.f));
	m_material->SetTexture(2, heightMap);

	shared_ptr<MeshRenderer> meshRenderer = GetGameObject()->GetMeshRenderer();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadTerrainMesh(sizeX, sizeZ);
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");
		meshRenderer->SetMaterial(material);
	}

	//std::ifstream file(L"..\\Resources\\Texture\\Terrain\\height.png", std::ios::binary);
	std::ifstream file(L"..\\Resources\\Texture\\Terrain\\pixels.bin", std::ios::binary);
	if (!file)
	{
		return;
	}
	unsigned char pixel;
	m_height = make_shared<vector<int>>();
	m_height->reserve(heightMap->GetWidth() * heightMap->GetHeight());
	while (file.read(reinterpret_cast<char*>(&pixel), sizeof(pixel))) {
		m_height->push_back(pixel);
	}
}

void Terrain::FinalUpdate()
{
	shared_ptr<Camera> mainCamera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	if (mainCamera == nullptr)
		return;

	Vec3 pos = mainCamera->GetTransform()->GetLocalPosition();
	m_material->SetVec4(0, Vec4(pos.x, pos.y, pos.z, 0));
}

float Terrain::getHeight(float fx, float fz)
{
	Vec3 scale = GetTransform()->GetLocalScale();
	float mapSizeX = scale.x * m_sizeX;
	float mapSizeZ = scale.z * m_sizeZ;
	int x = int(fx * m_texSizeX / mapSizeX);
	int z = int(fz * m_texSizeZ / mapSizeZ);
	fx = fx * m_texSizeX / mapSizeX;
	fz = fz * m_texSizeZ / mapSizeZ;

	if (x < 0 || x > m_texSizeX)
		return 0;

	if (z < 0 || z > m_texSizeZ)
		return 0;
	int index = x + m_texSizeX * (m_texSizeZ - 1 - z);

	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 0];
	float fBottomRight =	(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 1];
	float fTopLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 0];
	float fTopRight =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 1];



	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight / 256 * scale.y);

}
