#pragma once

static constexpr size_t BUFFER_SIZE = 1024;

class Packet;

class Buffer {
public:
	Buffer();
	~Buffer() {}

	void Clear();
	void Write(const vector<char>& data);
	void Write(const char* data, size_t size);

	char Peek() const;
	char Peek(int index) const;

	char Read();
	void Read(char* data, size_t size);
	void Read(shared_ptr<Packet>& packet);

	bool Empty() const;

	size_t Size() const { return m_size; }

private:
	void Init();

private:
	array<char, BUFFER_SIZE> m_buffer;
	size_t m_size;

	size_t m_readIndex;
	size_t m_writeIndex;
};

enum class PacketType : BYTE {
	PT_NONE = 0,
	PT_MOVE = 1,
	PT_MAX
};

class Packet {
public:
	Packet();
	~Packet() {}

public:
	short m_size;
	PacketType m_type;
	int m_targetId;
};

class MovePacket : public Packet {
public:
	MovePacket();
	~MovePacket() {}

public:
	Vec3 m_position;
};