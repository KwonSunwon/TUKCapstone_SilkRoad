#pragma once
#include "Component.h"

class NetworkObject : public Component {
public:
	NetworkObject();
	~NetworkObject();

	void ProcessPacket(shared_ptr<class Packet> packet);

	void SetNetworkId(uint32 id) { m_networkId = id; }
	uint32 GetNetworkId() { return m_networkId; }

private:
	uint32 m_networkId;
};