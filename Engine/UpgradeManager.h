#pragma once
class UpgradeManager
{
    DECLARE_SINGLE(UpgradeManager);

public:
    void Upgrade(int id);
    void Init();

private:

    array<int, 5> m_upgradeLevels;
};

