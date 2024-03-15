#pragma once

#include "MonoBehaviour.h"

class TestPlayer : public MonoBehaviour {
public:
	TestPlayer(int id);

	virtual void LateUpdate() override;

private:
	int m_id = 0;
};

