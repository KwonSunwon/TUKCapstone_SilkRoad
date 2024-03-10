#pragma once
#include "Component.h"

class Terrain : public Component
{
public:
	Terrain();
	virtual ~Terrain();

	void Init(int32 sizeX, int32 sizeZ);

	virtual void FinalUpdate() override;

	float getHeight(float, float);

private:
	int32 m_sizeX = 15;
	int32 m_sizeZ = 15;

	int32 m_texSizeX = 0;
	int32 m_texSizeZ = 0;
	float m_maxTesselation = 4.f;

	shared_ptr<class Material> m_material;
	shared_ptr<vector<int>> m_height;
};


