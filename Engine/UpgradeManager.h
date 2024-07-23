#pragma once

class UpgradeManager
{
    DECLARE_SINGLE(UpgradeManager);

public:
    void Upgrade(int id);
    void Init();
    void ClassChange(int id);
    void SetClass() { ClassChange(m_charClass); }


private:

    array<int, 5> m_upgradeLevels;
    int m_charClass = 5;
};

