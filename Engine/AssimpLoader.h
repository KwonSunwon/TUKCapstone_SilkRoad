#pragma once

struct AssimpMeshInfo
{
	//wstring								name;
	vector<Vertex>						vertices;
	vector<vector<uint32>>				indices;
};

class AssimpLoader
{
private:
	const aiScene* m_pScene;
	UINT m_numVertices;
	UINT m_numMaterial;

public:
	AssimpLoader();
	~AssimpLoader();

	void LoadFbx(const wstring& path);
	const aiScene* GetScene() { return m_pScene; };
};