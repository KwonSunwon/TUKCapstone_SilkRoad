#include "pch.h"
#include "AssimpLoader.h"

AssimpLoader::AssimpLoader()
{

}

AssimpLoader::~AssimpLoader()
{
    if (m_pScene)
        delete m_pScene;
}

void AssimpLoader::LoadFbx(const wstring& path)
{
    string filename = ws2s(path);

    m_pScene = aiImportFile(filename.c_str(), //aiProcess_JoinIdenticalVertices |        // ������ ������ ����, �ε��� ����ȭ

        aiProcess_ValidateDataStructure |        // �δ��� ����� ����

        aiProcess_ImproveCacheLocality |        // ��� ������ ĳ����ġ�� ����

        aiProcess_RemoveRedundantMaterials |    // �ߺ��� ���͸��� ����

        aiProcess_GenUVCoords |                    // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ

        aiProcess_TransformUVCoords |            // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)

        aiProcess_FindInstances |                // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����

        //aiProcess_LimitBoneWeights |            // ������ ���� ����ġ�� �ִ� 4���� ����

        //aiProcess_OptimizeMeshes |                // ������ ��� ���� �Ž��� ����

        //aiProcess_GenSmoothNormals |            // �ε巯�� �븻����(��������) ����

        //aiProcess_SplitLargeMeshes |            // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)

        aiProcess_CalcTangentSpace |                // ź��Ʈ ���� ����

        aiProcess_Triangulate |                    // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)

        aiProcess_ConvertToLeftHanded |            // D3D�� �޼���ǥ��� ��ȯ

        aiProcess_SortByPType);
}