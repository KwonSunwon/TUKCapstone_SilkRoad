#pragma once

class GameObject;
class BaseCollider;
class OcTree;

class Scene
{
public:
	Scene();
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<class Camera> GetMainCamera();

	void Render();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

	void RenderUI();

private:
	void PushLightData();

public:
	void SetPlayer(shared_ptr<GameObject> player) { m_player = player; }
	shared_ptr<GameObject> GetPlayer() { return m_player; }

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }


	void IntersectColliders(shared_ptr<BaseCollider> collider1, shared_ptr<BaseCollider> collider2);
	void testCollision();
	shared_ptr<GameObject> m_terrain;
private:
	shared_ptr<GameObject> m_player;

	vector<shared_ptr<GameObject>>		m_gameObjects;
	vector<shared_ptr<GameObject>>		m_collidableGameObjects;
	vector<shared_ptr<class Camera>>	m_cameras;
	vector<shared_ptr<class Light>>		m_lights;

	//test func
private:
	shared_ptr<OcTree> m_ocTree = make_shared<OcTree>(50000, 1000);
	
};

