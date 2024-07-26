#pragma once

enum PlayerType
{
	MAIN_PLAYER,
	GUEST_PLAYER1,
	GUEST_PLAYER2
};

class GameObject;
class BaseCollider;
class OcTree;
class Terrain;
class Player;
class Enemy;
class TextObject;
class AstarGrid;

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
	shared_ptr<class Light> GetMainLight();

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
	void Resolution();

public:
	void SetPlayer(shared_ptr<GameObject> player, PlayerType type) { m_players[type] = player; }
	array<shared_ptr<GameObject>, 3> GetPlayers() { return m_players; }

	void SetAstargrid(shared_ptr<AstarGrid> astarGrid) { m_astarGrid = astarGrid; }
	shared_ptr<AstarGrid> GetAstarGrid() { return m_astarGrid; }

	void SetMainPlayerScript(shared_ptr<Player> player) { m_mainPlayerScript = player; }
	shared_ptr<Player> GetMainPlayerScript() { return m_mainPlayerScript; }

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	void AddTextObject(shared_ptr<TextObject> textObject);
	void RemoveTextObject(shared_ptr<TextObject> textObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
	const vector<shared_ptr<GameObject>>& GetCollidableGameObjects() { return m_collidableGameObjects; }
	const vector<shared_ptr<GameObject>>& GetInteractiveGameObjects() { return m_interactiveGameObjects; }
	const vector<shared_ptr<TextObject>>& GetTextObjects() { return m_textObjects; }

	const vector<shared_ptr<class NetworkObject>>& GetNetworkObjects() { return m_networkObjects; }
	const array<shared_ptr<class NetworkPlayer>, 2>& GetNetworkPlayers() { return m_networkPlayers; }

	void IntersectColliders(shared_ptr<BaseCollider> collider1, shared_ptr<BaseCollider> collider2);
	void testCollision();
	void PhysicsStep(int iterations);

	void SetSceneName(string name) { m_sceneName = name; }
	string GetSceneName() { return m_sceneName; }

	shared_ptr<Terrain> m_terrain;

	array<shared_ptr<class NetworkPlayer>, 2> m_networkPlayers;
	array<shared_ptr<class Enemy>, 21> m_enemies;

	array<shared_ptr<GameObject>, 3> m_players;
	shared_ptr<Player> m_mainPlayerScript;
	shared_ptr<Enemy> m_bossMonsterScript;

	int AddNetworkObject() { return m_numOfNetworkObject++; }
private:
	vector<shared_ptr<GameObject>>		m_gameObjects;
	vector<shared_ptr<GameObject>>		m_collidableGameObjects;
	vector<shared_ptr<GameObject>>		m_interactiveGameObjects;
	vector<shared_ptr<TextObject>>		m_textObjects;
	vector<shared_ptr<class Camera>>	m_cameras;
	vector<shared_ptr<class Light>>		m_lights;
	vector<shared_ptr<class Manifold>>	m_contacts;
	// 네트워크 객체 목록 추가
	vector<shared_ptr<class NetworkObject>> m_networkObjects;

private:
	int m_numOfNetworkObject = 0;
	shared_ptr<OcTree> m_ocTree = make_shared<OcTree>(100000, 1000);
	shared_ptr<AstarGrid> m_astarGrid = make_shared<AstarGrid>();
	string m_sceneName = "";


};

