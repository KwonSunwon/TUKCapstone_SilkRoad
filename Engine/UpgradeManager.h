#pragma once

enum CharacterClass
{
    DEALER = 5,
    HEALER = 6,
    LAUNCHER = 7,
    TANKER = 8
};

class UpgradeManager
{
    DECLARE_SINGLE(UpgradeManager);

public:
    void Upgrade(int id);
    void ApplyItem(int id);
    void Init();
    void ClassChange(int id);
    void SetClass();
    void SetStat();

	int GetItemLevel(int id) { return m_itemLevels[id]; }
	int GetUpgradeLevel(int id) { return m_upgradeLevels[id]; }
	int GetClass() { return m_charClass; }
    int GetGold() { return m_upgradeLevels[5]; }
    void AddGold(int gold) { m_upgradeLevels[5] += gold; }
    void UseGold(int gold){ m_upgradeLevels[5] -= gold; }

private:

    array<int, 6> m_upgradeLevels;
    array<int, 17> m_itemLevels;
    int m_charClass = 5;

                                    
    array<array<float, 10>, 4> m_stat;
        

};

