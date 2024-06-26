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
	void Resolution();

public:
	void SetPlayer(shared_ptr<GameObject> player, PlayerType type) { m_players[type] = player; }
	array<shared_ptr<GameObject>, 3> GetPlayers() { return m_players; }

	void SetMainPlayerScript(shared_ptr<Player> player) { m_mainPlayerScript = player; }
	shared_ptr<Player> GetMainPlayerScript() { return m_mainPlayerScript; }

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }

	void IntersectColliders(shared_ptr<BaseCollider> collider1, shared_ptr<BaseCollider> collider2);
	void testCollision();
	void PhysicsStep(int iterations);

	shared_ptr<Terrain> m_terrain;

	array<shared_ptr<class NetworkPlayer>, 2> m_networkPlayers;
	array<shared_ptr<class Enemy>, 21> m_enemies;

	array<shared_ptr<GameObject>, 3> m_players;
	shared_ptr<Player> m_mainPlayerScript;
	shared_ptr<Enemy> m_bossMonsterScript;
private:
	vector<shared_ptr<GameObject>>		m_gameObjects;
	vector<shared_ptr<GameObject>>		m_collidableGameObjects;
	vector<shared_ptr<class Camera>>	m_cameras;
	vector<shared_ptr<class Light>>		m_lights;
	vector<shared_ptr<class Manifold>>	m_contacts;
private:
	shared_ptr<OcTree> m_ocTree = make_shared<OcTree>(100000, 1000);


};

