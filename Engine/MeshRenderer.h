#pragma once
#include "Component.h"

class Mesh;
class Material;

// [32][32]
union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};
	uint64 id;
};

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	shared_ptr<Mesh> GetMesh() { return m_mesh; }
	shared_ptr<Material> GetMaterial(uint32 idx = 0) { return m_materials[idx]; }

	void SetMesh(shared_ptr<Mesh> mesh) { m_mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material, uint32 idx = 0);

	void Render();
	void Render(shared_ptr<class InstancingBuffer>& buffer);
	void RenderShadow();
	void RenderShadow(shared_ptr<class InstancingBuffer>& buffer);

	uint64 GetInstanceID();

private:
	shared_ptr<Mesh> m_mesh;
	vector<shared_ptr<Material>> m_materials;
};

