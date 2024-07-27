#pragma once
#include "InteractiveObject.h"
class StagePortal : public InteractiveObject
{
public:
	virtual void Update() override;
	virtual void PrintInteractiveText() override;
	virtual void InteractiveFunction() override;

private:
	bool m_isActivated = false;
};

