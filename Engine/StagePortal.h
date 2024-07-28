#pragma once
#include "InteractiveObject.h"
class StagePortal : public InteractiveObject
{
public:
	StagePortal() : InteractiveObject(EnumInteract::STAGE_PORTAL) {}

	virtual void Update() override;
	virtual void PrintInteractiveText() override;
	virtual void InteractiveFunction() override;

private:
	bool m_isActivated = false;
};

