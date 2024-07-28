#include "pch.h"
#include "Packet.h"
#include "Scene.h"
#include "SceneManager.h"

Buffer::Buffer()
{
	Init();
}

void Buffer::Clear()
{
	Init();
}

void Buffer::Init()
{
	m_buffer.fill(-1);
	m_size = 0;

	m_readIndex = 0;
	m_writeIndex = 0;
}

void Buffer::Write(const vector<char>& data)
{
	for(size_t i = 0; i < data.size(); i++) {
		m_buffer[m_writeIndex] = data[i];
		m_writeIndex = (m_writeIndex + 1) % BUFFER_SIZE;
	}

	m_size += data.size();
}

void Buffer::Write(const char* data, size_t size)
{
	for(size_t i = 0; i < size; i++) {
		m_buffer[m_writeIndex] = data[i];
		m_writeIndex = (m_writeIndex + 1) % BUFFER_SIZE;
	}

	m_size += size;
}

char Buffer::Peek() const
{
	if(Empty())
		return -1;
	return m_buffer[m_readIndex];
}

char Buffer::Peek(int index) const
{
	if(Empty())
		return -1;
	return m_buffer[(m_readIndex + index) % BUFFER_SIZE];
}

char Buffer::Read()
{
	if(Empty())
		return -1;

	char data = m_buffer[m_readIndex];
	m_readIndex = (m_readIndex + 1) % BUFFER_SIZE;
	m_size--;

	return data;
}

bool Buffer::Read(char* data, size_t size)
{
	if(Empty())
		return false;

	if(size > m_size)
		return false;

	for(size_t i = 0; i < size; i++) {
		data[i] = m_buffer[m_readIndex];
		m_readIndex = (m_readIndex + 1) % BUFFER_SIZE;
	}

	m_size -= size;
	return true;
}

void Buffer::Read(shared_ptr<Packet>& packet)
{
	if(Empty())
		return;
	if(!packet)
		return;

	if(packet->m_size == sizeof(packet)) {
		return;
	}

	if(packet->m_size > BUFFER_SIZE - m_readIndex) {
		memcpy(packet.get(), m_buffer.data() + m_readIndex, BUFFER_SIZE - m_readIndex);
		memcpy(packet.get() + BUFFER_SIZE - m_readIndex, m_buffer.data(), packet->m_size - (BUFFER_SIZE - m_readIndex));
	}
	else {
		memcpy(packet.get(), m_buffer.data() + m_readIndex, packet->m_size);
	}

	m_size -= packet->m_size;
	m_readIndex = (m_readIndex + packet->m_size) % BUFFER_SIZE;
}

bool Buffer::Empty() const
{
	return m_size == 0;
}

Packet::Packet()
{
	m_targetId = -1;
	m_stageIndex = GET_SINGLE(SceneManager)->GetStageIndex();
}

InitPacket::InitPacket() : Packet()
{
	m_size = sizeof(InitPacket);
	m_type = PACKET_TYPE::PT_INIT;
}

PlayerPacket::PlayerPacket() : Packet()
{
	m_size = sizeof(PlayerPacket);
	m_type = PACKET_TYPE::PT_PLAYER;
}

EnemyPacket::EnemyPacket() : Packet()
{
	m_size = sizeof(EnemyPacket);
	m_type = PACKET_TYPE::PT_ENEMY;
}

EnemyHitPacket::EnemyHitPacket() : Packet()
{
	m_size = sizeof(EnemyHitPacket);
	m_type = PACKET_TYPE::PT_ENEMY_HIT;
}

StageChangePacket::StageChangePacket() : Packet()
{
	m_size = sizeof(StageChangePacket);
	m_type = PACKET_TYPE::PT_STAGE_CHANGE;
}

PlayerClassChangePacket::PlayerClassChangePacket() : Packet()
{
	m_size = sizeof(PlayerClassChangePacket);
	m_type = PACKET_TYPE::PT_PLAYER_CLASS_CHANGE;
}

SkillPacket::SkillPacket() : Packet()
{
	m_size = sizeof(SkillPacket);
	m_type = PACKET_TYPE::PT_SKILL;
}

ItemPacket::ItemPacket() : Packet()
{
	m_size = sizeof(ItemPacket);
	m_type = PACKET_TYPE::PT_ITEM;
}

GuestInitPacket::GuestInitPacket() : Packet()
{
	m_size = sizeof(GuestInitPacket);
	m_type = PACKET_TYPE::PT_GUEST_INIT;
}

ParticlePacket::ParticlePacket() : Packet()
{
	m_size = sizeof(ParticlePacket);
	m_type = PACKET_TYPE::PT_PARTICLE;
}

ForcePacket::ForcePacket() : Packet()
{
	m_size = sizeof(ForcePacket);
	m_type = PACKET_TYPE::PT_FORCE;
}

PlayerHitPacket::PlayerHitPacket() : Packet()
{
	m_size = sizeof(PlayerHitPacket);
	m_type = PACKET_TYPE::PT_PLAYER_HIT;
}

PortalOnPacket::PortalOnPacket() : Packet()
{
	m_size = sizeof(PortalOnPacket);
	m_type = PACKET_TYPE::PT_PORTAL_ON;
}
