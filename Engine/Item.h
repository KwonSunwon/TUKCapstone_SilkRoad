#pragma once
#include "MonoBehaviour.h"

#define PI 3.141592 

class Item : public MonoBehaviour, public std::enable_shared_from_this<Item>
{
	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetItemId(int id) { m_itemId = id; }
	int GetItemID() { return m_itemId; }
	void AddGetItemText();
private:
	void FloatingItem();
	void RotateItem();
	
	

private:
	string	m_itemName;
	int		m_itemId = -1;
	float	m_startHeight = 0;
	float	m_radian = 0.f;
	float	m_frequency = 0.5f;
	float	m_amplitude = 50.f;

};

