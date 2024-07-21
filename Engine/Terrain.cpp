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
#include  "Timer.h"


Terrain::Terrain() : Component(COMPONENT_TYPE::TERRAIN)
{

}

Terrain::~Terrain()
{
}

void Terrain::Init(int32 sizeX, int32 sizeZ, int stage)
{
	


	m_sizeX = sizeX;
	m_sizeZ = sizeZ;

	m_material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");

	m_material->SetInt(1, m_sizeX);
	m_material->SetInt(2, m_sizeZ);
	m_material->SetFloat(0, m_maxTesselation);

	wstring diffuseName, diffusePath, heightMapName, heightMapPath, binPath;

	switch (stage)
	{
	case 1:
		diffuseName = L"Terrain";
		diffusePath = L"..\\Resources\\Texture\\Terrain\\terrain.png";
		heightMapName = L"HeightMap";
		heightMapPath = L"..\\Resources\\Texture\\Terrain\\height.png";
		binPath = L"..\\Resources\\Texture\\Terrain\\pixels.bin";
		break;

	case 2:
		diffuseName = L"Terrain2";
		diffusePath = L"..\\Resources\\Texture\\Terrain\\terrain2.png";
		heightMapName = L"HeightMap2";
		heightMapPath = L"..\\Resources\\Texture\\Terrain\\height2.png";
		binPath = L"..\\Resources\\Texture\\Terrain\\pixels.bin";
		break;

	default:
		diffuseName = L"Terrain";
		diffusePath = L"..\\Resources\\Texture\\Terrain\\terrain.png";
		heightMapName = L"HeightMap";
		heightMapPath = L"..\\Resources\\Texture\\Terrain\\height.png";
		binPath = L"..\\Resources\\Texture\\Terrain\\pixels.bin";
	}

	shared_ptr<Texture> diffuse = GET_SINGLE(Resources)->Load<Texture>(diffuseName, diffusePath);
	shared_ptr<Texture> heightMap = GET_SINGLE(Resources)->Load<Texture>(heightMapName, heightMapPath);
	
	m_material->SetTexture(0, diffuse);
	
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

	std::ifstream file(binPath, std::ios::binary);
	
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



	{
		Vec3 scale = GetTransform()->GetLocalScale();
		float mapSizeX = scale.x * m_sizeX;
		float mapSizeZ = scale.z * m_sizeZ;
		float blockX = mapSizeX / m_texSizeX;
		float blockZ = mapSizeZ / m_texSizeZ;
		shared_ptr<Vec3> norm = make_shared<Vec3>();
		shared_ptr<float> h = make_shared < float >();

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

void Terrain::Awake()
{
	

	
	
	

}

void Terrain::getHeight(float fx, float fz , shared_ptr<float> height, shared_ptr<Vec3> terrainNormal)
{


	Vec3 scale = GetTransform()->GetLocalScale();

	//전체 터레인 크기
	float mapSizeX = scale.x * m_sizeX;
	float mapSizeZ = scale.z * m_sizeZ;

	//높이맵의 인덱스 접근
	int x = int(fx * m_texSizeX / mapSizeX);
	int z = int(fz * m_texSizeZ / mapSizeZ);

	//높이맵의 인덱스에서 비율
	fx = fx * m_texSizeX / mapSizeX;
	fz = fz * m_texSizeZ / mapSizeZ;

	//높이맵의 한 픽셀의 실제 게임 월드에서 크기
	float blockX = mapSizeX / m_texSizeX;
	float blockZ = mapSizeZ / m_texSizeZ;

	//예외처리
	int index = x + m_texSizeX * (m_texSizeZ - 1 - z);
	if (x < 0 || x > m_texSizeX)
		return;
	if (z < 0 || z+2 > m_texSizeZ)
		return;
	if (index+1 >= m_height->size())
		return;


	//해당 위치에서의 주변 점 높이 구하기
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 0];
	float fBottomRight =	(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 1];
	float fTopLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 0];
	float fTopRight =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 1];

	//법선벡터를 찾기 위해 포함되는 삼각형 찾기
	Vec3 edgeA, edgeB;
	if (fxPercent + fzPercent < 1) {
		edgeA = Vec3(blockX, (fBottomRight - fBottomLeft) / 255 * scale.y, 0.f);
		edgeB = Vec3(0.f, (fTopLeft - fBottomLeft) / 255 * scale.y, blockZ);
	}
	else {
		edgeA = Vec3(-blockX, (fTopLeft - fTopRight) / 255 * scale.y, 0.f);
		edgeB = Vec3(0.f, (fBottomRight - fTopRight) / 255 * scale.y, -blockZ);
	}

	//법선벡터 계산
	*terrainNormal = edgeA.Cross(edgeB);
	if ((*terrainNormal).y < 0.f)
		(*terrainNormal) = -(*terrainNormal);
	(*terrainNormal).Normalize();

	//높이의 선형보간을 위해 포함되는 삼각형 찾기
	if (fxPercent + fzPercent < 1)fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
	else fBottomLeft = fTopLeft + (fBottomRight - fTopRight);

	//높이 계산
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	//높이를 0~1로 정규화 시켜준뒤 맵의 크기로 곱해준다.
	*height = (fHeight / 255 * scale.y);

}

