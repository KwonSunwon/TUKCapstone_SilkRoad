#pragma once
class UpgradeManager
{
    DECLARE_SINGLE(UpgradeManager);

public:
    void Upgrade(int id);
    void Init();
    void ClassChange(int id);

private:

    array<int, 5> m_upgradeLevels;
};

