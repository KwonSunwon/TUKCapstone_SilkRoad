#pragma once
#include "ParticleSystem.h"
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
class InteractiveObjectText;

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

	void SetInteractiveObjectText(shared_ptr<InteractiveObjectText> textObject) { m_interactiveObjectText = textObject; }
	shared_ptr<InteractiveObjectText> GetInteractiveObjectText() { return m_interactiveObjectText; }

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	void AddTextObject(shared_ptr<TextObject> textObject);
	void RemoveTextObject(shared_ptr<TextObject> textObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
	const vector<shared_ptr<GameObject>>& GetCollidableGameObjects() { return m_collidableGameObjects; }
	const vector<shared_ptr<GameObject>>& GetInteractiveGameObjects() { return m_interactiveGameObjects; }
	const vector<shared_ptr<TextObject>>& GetTextObjects() { return m_textObjects; }
	const vector<shared_ptr<class Enemy>>& GetLastWaveEnemies() { return m_lastWaveEnemies; }
	void AddLastWaveEnemy(shared_ptr<class Enemy> enemy) { m_lastWaveEnemies.push_back(enemy); }

	const vector<shared_ptr<class NetworkObject>>& GetNetworkObjects() { return m_networkObjects; }
	const array<shared_ptr<class NetworkPlayer>, 2>& GetNetworkPlayers() { return m_networkPlayers; }

	void IntersectColliders(shared_ptr<BaseCollider> collider1, shared_ptr<BaseCollider> collider2);
	void testCollision();
	void PhysicsStep(int iterations);

	void SetSceneName(string name) { m_sceneName = name; }
	string GetSceneName() { return m_sceneName; }

	void SetStagePortal(shared_ptr<class StagePortal> portal) { m_stagePortal = portal; }
	shared_ptr<class StagePortal> GetStagePortal() { return m_stagePortal; }

	shared_ptr<Terrain> m_terrain;

	array<shared_ptr<class NetworkPlayer>, 2> m_networkPlayers;
	array<shared_ptr<class Enemy>, 21> m_enemies;

	array<shared_ptr<GameObject>, 3> m_players;
	shared_ptr<Player> m_mainPlayerScript;
	shared_ptr<Enemy> m_bossMonsterScript;
	shared_ptr<InteractiveObjectText> m_interactiveObjectText;

	int AddNetworkObject() { return m_numOfNetworkObject++; }

	bool ChangeSpectate(PlayerType type);
	shared_ptr<GameObject> SpawnParticle(Vec3 pos, int type, bool network = false);
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

	vector<shared_ptr<class Enemy>>		m_lastWaveEnemies;

	array<vector<shared_ptr<GameObject>>, ParticleType::PARTICLE_TYPE_COUNT>	m_particles;
	array<int, ParticleType::PARTICLE_TYPE_COUNT>m_particleCycle = { 0 };

	shared_ptr<class StagePortal> m_stagePortal;

private:
	int m_numOfNetworkObject = 0;
	shared_ptr<AstarGrid> m_astarGrid = make_shared<AstarGrid>();
	string m_sceneName = "";


};

