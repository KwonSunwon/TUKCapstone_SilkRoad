#include "pch.h"
#include "UpgradeManager.h"
#include "SoundManager.h"

void UpgradeManager::Init()
{
	std::ifstream upgradeFile("upgrade.txt");
	if (upgradeFile.is_open()) {
		for (int i = 0; i < 5; ++i) {
			// 파일에서 정수를 읽어와 배열에 저장
			upgradeFile >> m_upgradeLevels[i];
		}
		upgradeFile.close();
	}
}

void UpgradeManager::Upgrade(int id)
{	//돈이 있다면
	if (1) {
		//테이블에 따라 돈을 줄이고,

		GET_SINGLE(SoundManager)->soundPlay(Sounds::ENV_EAT_ITEM);
		m_upgradeLevels[id]++;
		ofstream upgradeFile("upgrade.txt");
		if (upgradeFile.is_open()) {
			for (int i = 0; i < 5; ++i) {
				upgradeFile << m_upgradeLevels[i] << std::endl;
			}
			upgradeFile.close();
		}
	}
}


