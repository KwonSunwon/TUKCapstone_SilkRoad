#pragma once

static const size_t BUFFER_SIZE = 2048;

class Buffer {
public:
	Buffer();
	~Buffer() {}

	void Clear();
	void Write(const vector<char> data);
	void Read(char& data);
	void Read(vector<char>& data, size_t size);

	bool Empty() const;

private:
	void Init();

private:
	array<char, BUFFER_SIZE> m_buffer;
	size_t m_size;

	int m_readIndex;
	int m_writeIndex;

public:
	int m_remainData;
};

enum class PacketType : BYTE {
	PT_NONE = 0,
	PT_MOVE = 1,
	PT_MAX
};

struct MovePacket {
	ushort size;
	BYTE type;

};