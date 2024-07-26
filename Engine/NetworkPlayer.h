#pragma once
#include "MonoBehaviour.h"
#include "Packet.h"

class NetworkPlayer : public MonoBehaviour
{
public:
	NetworkPlayer() {};
	virtual ~NetworkPlayer() {};

	virtual void Awake();
	virtual void Update();
	virtual void LateUpdate();

	void ProcessPacket(shared_ptr<PlayerPacket> packet);
	void ChangeClass(int classIndex);

	void SetHP(float hp) { m_hp = hp; }
	float GetHP() { return m_hp; }

	void SetActivated(bool isActivated) { m_isActivated = isActivated; }
	bool IsActivated() { return m_isActivated; }

	void SetClassIndex(int classIndex) { m_classIndex = classIndex; }
	int GetClassIndex() { return m_classIndex; }

public:
	uint32 m_myNetworkId = -1;

private:
	float m_hp = 100.f;
	bool m_isActivated = false;
	int m_classIndex = 0;
};