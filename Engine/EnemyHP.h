#pragma once
#include "MonoBehaviour.h"
class Enemy;

class EnemyHP : public MonoBehaviour
{
public:
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	void SetParentEnemy(shared_ptr<Enemy> pe) { m_parentEnemy = pe; }

private:
	shared_ptr<Enemy> m_parentEnemy;
	
	
};
