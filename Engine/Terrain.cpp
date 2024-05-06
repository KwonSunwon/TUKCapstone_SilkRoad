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

	//��ü �ͷ��� ũ��
	float mapSizeX = scale.x * m_sizeX;
	float mapSizeZ = scale.z * m_sizeZ;

	//���̸��� �ε��� ����
	int x = int(fx * m_texSizeX / mapSizeX);
	int z = int(fz * m_texSizeZ / mapSizeZ);

	//���̸��� �ε������� ����
	fx = fx * m_texSizeX / mapSizeX;
	fz = fz * m_texSizeZ / mapSizeZ;

	//���̸��� �� �ȼ��� ���� ���� ���忡�� ũ��
	float blockX = mapSizeX / m_texSizeX;
	float blockZ = mapSizeZ / m_texSizeZ;

	//����ó��
	int index = x + m_texSizeX * (m_texSizeZ - 1 - z);
	if (x < 0 || x > m_texSizeX)
		return;
	if (z < 0 || z+2 > m_texSizeZ)
		return;
	if (index+1 >= m_height->size())
		return;


	//�ش� ��ġ������ �ֺ� �� ���� ���ϱ�
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 0];
	float fBottomRight =	(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 1 - z) + 1];
	float fTopLeft =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 0];
	float fTopRight =		(float)(*m_height)[x + m_texSizeX * (m_texSizeZ - 2 - z) + 1];

	//�������͸� ã�� ���� ���ԵǴ� �ﰢ�� ã��
	Vec3 edgeA, edgeB;
	if (fxPercent + fzPercent < 1) {
		edgeA = Vec3(blockX, (fBottomRight - fBottomLeft) / 255 * scale.y, 0.f);
		edgeB = Vec3(0.f, (fTopLeft - fBottomLeft) / 255 * scale.y, blockZ);
	}
	else {
		edgeA = Vec3(-blockX, (fTopLeft - fTopRight) / 255 * scale.y, 0.f);
		edgeB = Vec3(0.f, (fBottomRight - fTopRight) / 255 * scale.y, -blockZ);
	}

	//�������� ���
	*terrainNormal = edgeA.Cross(edgeB);
	if ((*terrainNormal).y < 0.f)
		(*terrainNormal) = -(*terrainNormal);
	(*terrainNormal).Normalize();

	//������ ���������� ���� ���ԵǴ� �ﰢ�� ã��
	if (fxPercent + fzPercent < 1)fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
	else fBottomLeft = fTopLeft + (fBottomRight - fTopRight);

	//���� ���
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	//���̸� 0~1�� ����ȭ �����ص� ���� ũ��� �����ش�.
	*height = (fHeight / 255 * scale.y);

}

