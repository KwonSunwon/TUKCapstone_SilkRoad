#pragma once

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

private:

    array<int, 5> m_upgradeLevels;
    array<int, 17> m_itemLevels;
    int m_charClass = 5;

                                    
    array<array<float, 10>, 4> m_stat;
        

};

