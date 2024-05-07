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

	uint32 m_myNetworkId = -1;

};