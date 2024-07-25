#include "pch.h"
#include "Packet.h"

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
	m_size = 0;
	m_type = PACKET_TYPE::PT_NONE;
	m_targetId = -1;
}

InitPacket::InitPacket()
{
	m_size = sizeof(InitPacket);
	m_type = PACKET_TYPE::PT_INIT;
	m_targetId = -1;
}

MovePacket::MovePacket()
{
	m_size = sizeof(MovePacket);
	m_type = PACKET_TYPE::PT_MOVE;
	m_targetId = -1;
}

PlayerPacket::PlayerPacket()
{
	m_size = sizeof(PlayerPacket);
	m_type = PACKET_TYPE::PT_PLAYER;
	m_targetId = -1;
}

EnemyPacket::EnemyPacket()
{
	m_size = sizeof(EnemyPacket);
	m_targetId = -1;
	m_type = PACKET_TYPE::PT_ENEMY;
}
