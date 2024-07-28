#pragma once
#include "Component.h"

class NetworkObject : public Component {
public:
	NetworkObject();
	~NetworkObject();

	void LateUpdate() override;

	void ProcessPacket(shared_ptr<class Packet> packet);

	void SetNetworkId(uint32 id) { m_networkId = id; }
	uint32 GetNetworkId() { return m_networkId; }

	void SetDefaultPacket(bool isDefaultPacket) { m_isDefaultPacket = isDefaultPacket; }
	bool IsDefaultPacket() { return m_isDefaultPacket; }

	void SetActive(bool isActivated) { m_isActive = isActivated; }
	bool IsActive() { return m_isActive; }

private:
	uint32 m_networkId;
	bool m_isDefaultPacket = false;
	bool m_isActive = true;
};