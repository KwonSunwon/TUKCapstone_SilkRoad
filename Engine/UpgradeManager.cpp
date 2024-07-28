#include "pch.h"
#include "UpgradeManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "MeshData.h"
#include "Mesh.h"
#include "InteractiveObject.h"
#include "Animator.h"
#include "Transform.h"
#include "Packet.h"
#include "Network.h"

void UpgradeManager::Init()
{//hp,maxHp,bulletDamage, hpregen, maxwalkSpeed, maxJumpSpeed, maxAimSpeed, cri percent, cri damage, m_fireRate
	m_stat = { {
		{100,100,20,2,1300,1000,500,0.3,2.0,3},
		{100,100,10,3,1300,1000,500,0.3,1.5,4},
		{100,100,50,2,1100,850,400,0.4,2.3,1},
		{150,150,10,3,1100,850,400,0.2,1.5,2.5}
	} };

	std::ifstream upgradeFile("upgrade.txt");
	if(upgradeFile.is_open()) {
		for(int i = 0; i < 6; ++i) {
			// 파일에서 정수를 읽어와 배열에 저장
			upgradeFile >> m_upgradeLevels[i];
		}
		upgradeFile.close();
	}
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Dealer.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Healer.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Launcher.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Tanker.fbx");
}

void UpgradeManager::ClassChange(int id)
{
	m_usedClass[m_charClass - 5] = true;
	m_usedClass[id - 5] = false;

	GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);
	shared_ptr<GameObject> mainPlayer = GET_SINGLE(SceneManager)->GetActiveScene()->m_mainPlayerScript->GetGameObject();
	shared_ptr<MeshData> meshData;

	switch(id)
	{
	case DEALER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Dealer.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 180.f, 40.f));
		break;

	case HEALER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Healer.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 170.f, 40.f));
		break;

	case LAUNCHER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Launcher.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 190.f, 40.f));
		break;

	case TANKER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Tanker.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 165.f, 40.f));
		break;

	default:
		break;
	}
	m_charClass = id;
	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
	mainPlayer->GetMeshRenderer()->SetMesh(gameObjects[0]->GetMeshRenderer()->GetMesh());
	mainPlayer->AddComponent(gameObjects[0]->GetAnimator());

	SetStat();

	if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
		shared_ptr<PlayerClassChangePacket> packet = make_shared<PlayerClassChangePacket>();
		packet->m_targetId = GET_SINGLE(NetworkManager)->m_networkId;
		packet->m_classIndex = id;
		SEND(packet);
	}
}

void UpgradeManager::SetClass()
{
	shared_ptr<GameObject> mainPlayer = GET_SINGLE(SceneManager)->GetActiveScene()->m_mainPlayerScript->GetGameObject();
	shared_ptr<MeshData> meshData;

	switch(m_charClass)
	{
	case DEALER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Dealer.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 180.f, 40.f));
		break;

	case HEALER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Healer.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 170.f, 40.f));
		break;

	case LAUNCHER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Launcher.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 190.f, 40.f));
		break;

	case TANKER:
		meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character_Tanker.fbx");
		GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform()->SetLocalPosition(Vec3(0.f, 165.f, 40.f));
		break;

	default:
		break;
	}

	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
	mainPlayer->GetMeshRenderer()->SetMesh(gameObjects[0]->GetMeshRenderer()->GetMesh());
	mainPlayer->AddComponent(gameObjects[0]->GetAnimator());

	SetStat();

	if(GET_SINGLE(NetworkManager)->GetNetworkState() != NETWORK_STATE::SINGLE) {
		shared_ptr<PlayerClassChangePacket> packet = make_shared<PlayerClassChangePacket>();
		packet->m_targetId = GET_SINGLE(NetworkManager)->m_networkId;
		packet->m_classIndex = m_charClass;
		SEND(packet);
	}
}
//hp,maxHp,bulletDamage, hpregen, maxwalkSpeed, maxJumpSpeed, maxAimSpeed, cri percent, cri damage, m_fireRate
void UpgradeManager::SetStat()
{
	shared_ptr<Player> mainPlayer = GET_SINGLE(SceneManager)->GetActiveScene()->m_mainPlayerScript;

	//기본 스탯
	float maxHP = m_stat[m_charClass - 5][1];
	float bulletDamage = m_stat[m_charClass - 5][2];
	float hpRegen = m_stat[m_charClass - 5][3];
	float maxWalkSpeed = m_stat[m_charClass - 5][4];
	float maxJumpSpeed = m_stat[m_charClass - 5][5];
	float maxAimSpeed = m_stat[m_charClass - 5][6];
	float criPercent = m_stat[m_charClass - 5][7];
	float criDamage = m_stat[m_charClass - 5][8];
	float fireRate = m_stat[m_charClass - 5][9];
	float minusDamage = 0.2f;
	float plusDamage = 0.2f;


	//아이템 스탯
	//m_itemLevels[0]  
	fireRate += m_itemLevels[1] * 1.f;
	bulletDamage += m_itemLevels[2] * 4.f;
	//m_itemLevels[3]
	//m_itemLevels[4]
	maxHP += m_itemLevels[5] * 20.f;
	maxWalkSpeed += m_itemLevels[6] * 200.f;
	maxJumpSpeed += m_itemLevels[7] * 150.f;
	maxAimSpeed += m_itemLevels[8] * 150.f;
	minusDamage += m_itemLevels[9] * (-0.01f);
	plusDamage += m_itemLevels[10] * 0.02f;
	criPercent += m_itemLevels[11] * 0.05f;
	criDamage += m_itemLevels[12] * 0.2f;
	//m_itemLevels[13]
	//m_itemLevels[14]
	//m_itemLevels[15]
	//m_itemLevels[16]


	//업그레이드 스탯
	maxHP += m_upgradeLevels[0] * 10.f;
	bulletDamage += m_upgradeLevels[1] * 2.f;
	fireRate += m_upgradeLevels[2] * 0.2f;
	criDamage += m_upgradeLevels[3] * 0.05f;
	hpRegen += m_upgradeLevels[4] * 0.4f;



	mainPlayer->SetMaxHP(maxHP);
	mainPlayer->SetBulletDamage(bulletDamage);
	mainPlayer->SetHpRegen(hpRegen);
	mainPlayer->SetMaxWalkSpeed(maxWalkSpeed);
	mainPlayer->SetMaxJumpSpeed(maxJumpSpeed);
	mainPlayer->SetMaxAimSpeed(maxAimSpeed);
	mainPlayer->SetCriPercent(criPercent);
	mainPlayer->SetCriDamage(criDamage);
	mainPlayer->SetFireRate(fireRate);
	mainPlayer->SetMinusDamage(minusDamage);
	mainPlayer->SetPlusDamage(plusDamage);

}

void UpgradeManager::Upgrade(int id)
{	
	int necessaryGold = (m_upgradeLevels[id] + 1) * 20;
	bool haveEnoughGold = GetGold() >= necessaryGold;

	if(haveEnoughGold) {
		UseGold(necessaryGold);

		GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);
		m_upgradeLevels[id]++;
		ofstream upgradeFile("upgrade.txt");
		if(upgradeFile.is_open()) {
			for(int i = 0; i < 6; ++i) {
				upgradeFile << m_upgradeLevels[i] << std::endl;
			}
			upgradeFile.close();
		}
	}
	SetStat();
}

void UpgradeManager::ApplyItem(int id)
{
	m_itemLevels[id]++;
	SetStat();
}


