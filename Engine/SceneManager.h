#pragma once

class D3D11On12Device;

class Scene;

enum
{
	MAX_LAYER = 32
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void RenderUI(shared_ptr<D3D11On12Device> d3d11On12device);
	void LoadScene(wstring sceneName);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return m_layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);


	void StartStage(int stage) { s = stage; reset = true; }
	void StartNextStage() { s++; reset = true; }
	void ResetStage() { reset = true; }

	int GetStageIndex() { return s; }

public:
	shared_ptr<Scene> GetActiveScene() { return m_activeScene; }

private:

	shared_ptr<Scene> LoadL();
	void LoadTextBrush();
	void LoadTextFormats();

private:
	shared_ptr<Scene> m_activeScene;

	array<wstring, MAX_LAYER> m_layerNames;
	map<wstring, uint8> m_layerIndex;

	int m_iterations = 1;
	int s = -1;
	bool reset = false;
};

