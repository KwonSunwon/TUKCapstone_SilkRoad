#pragma once
#include "InteractiveObject.h"
class StagePortal : public InteractiveObject
{
public:
	StagePortal() : InteractiveObject(EnumInteract::STAGE_PORTAL) {}

	virtual void Update() override;
	virtual void PrintInteractiveText() override;
	virtual void InteractiveFunction() override;

	bool m_isPacketProcess = false;

private:
	bool m_isActivated = false;
	float m_spawnTimer = 0.f;
	int m_spawnIdx = 0;
	bool m_spawnP = false;
};

