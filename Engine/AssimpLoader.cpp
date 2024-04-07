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

    m_pScene = aiImportFile(filename.c_str(), //aiProcess_JoinIdenticalVertices |        // 동일한 꼭지점 결합, 인덱싱 최적화

        aiProcess_ValidateDataStructure |        // 로더의 출력을 검증

        aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선

        aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거

        aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환

        aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)

        aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거

        //aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한

        //aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인

        //aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성

        //aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)

        aiProcess_CalcTangentSpace |                // 탄젠트 공간 생성

        aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)

        aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환

        aiProcess_SortByPType);
}