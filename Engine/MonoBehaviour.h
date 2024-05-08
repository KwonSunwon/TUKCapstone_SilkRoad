#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

	void SetMonovihaviourName(string name) { m_monovihaviourName = name; }
	string GetMonovihaviourName() { return m_monovihaviourName; }

private:
	virtual void FinalUpdate() sealed { }
	string m_monovihaviourName;
};

