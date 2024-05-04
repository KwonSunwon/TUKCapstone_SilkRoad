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

void Buffer::Write(const vector<char> data)
{
	for(size_t i = 0; i < data.size(); i++) {
		m_buffer[m_writeIndex] = data[i];
		m_writeIndex = (m_writeIndex + 1) % BUFFER_SIZE;
	}

	m_size += data.size();
}

void Buffer::Read(char& data)
{
	if(Empty())
		return;

	data = m_buffer[m_readIndex];
	m_readIndex = (m_readIndex + 1) % BUFFER_SIZE;
	m_size--;
}

void Buffer::Read(vector<char>& data, size_t size)
{
	if(Empty())
		return;

	if(size > m_size)
		size = m_size;

	for(size_t i = 0; i < size; i++) {
		data[i] = m_buffer[m_readIndex];
		m_readIndex = (m_readIndex + 1) % BUFFER_SIZE;
	}

	m_size -= size;
}

bool Buffer::Empty() const
{
	return m_size == 0;
}