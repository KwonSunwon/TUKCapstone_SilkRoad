#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

	string name;

private:
	virtual void FinalUpdate() sealed { }
};

