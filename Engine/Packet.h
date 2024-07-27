#pragma once

static constexpr size_t BUFFER_SIZE = 4096;

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
	bool Read(char* data, size_t size);
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

enum class PACKET_TYPE : BYTE {
	PT_NONE = 0,
	PT_INIT = 1,
	PT_GUEST_INIT,
	PT_PLAYER,
	PT_PLAYER_CLASS_CHANGE,
	PT_ENEMY,
	PT_ENEMY_HIT,
	PT_STAGE_CHANGE,
	PT_SKILL,
	PT_ITEM,
	PT_PARTICLE,
	PT_FORCE,
	PT_MAX,
};

#pragma pack(push, 1)
class Packet {
public:
	Packet();
	~Packet() {}

public:
	uint16 m_size;
	PACKET_TYPE m_type;
	uint32 m_targetId;
};

class InitPacket : public Packet {
public:
	InitPacket();
	~InitPacket() {}

public:
	BYTE m_networkId;
	int16 m_classIndex;
	int16 m_classIndexGuest;
};

class GuestInitPacket : public Packet {
public:
	GuestInitPacket();
	~GuestInitPacket() {}

public:
	int16 m_classIndex;
};

class PlayerPacket : public Packet {
public:
	PlayerPacket();
	~PlayerPacket() {}

public:
	Vec3 m_position;
	Vec3 m_velocity;
	Vec3 m_rotation;
	uint32 m_animationIndex;

	float m_hp;
};

class PlayerClassChangePacket : public Packet {
public:
	PlayerClassChangePacket();
	~PlayerClassChangePacket() {}

public:
	int32 m_classIndex;
};

class EnemyPacket : public Packet {
public:
	EnemyPacket();
	~EnemyPacket() {}

public:
	Vec3 m_position;
	Vec3 m_velocity;
	Vec3 m_rotation;
	uint32 m_animationIndex;

	uint32 m_targetPlayerId;
	float m_hp;
};

class EnemyHitPacket : public Packet {
public:
	EnemyHitPacket();
	~EnemyHitPacket() {}

public:
	float m_damage;
	Vec3 m_rayDir;
	float m_knockBackPower;
};

class StageChangePacket : public Packet {
public:
	StageChangePacket();
	~StageChangePacket() {}

public:
	uint32 m_stageIndex;
};

class SkillPacket : public Packet {
public:
	SkillPacket();
	~SkillPacket() {}

public:
	uint8 m_skillType;

	Vec3 m_pos;
	Vec3 m_look;
	Vec3 m_dropPos;

	bool m_isBomb;
};

class ItemPacket : public Packet {
public:
	ItemPacket();
	~ItemPacket() {}

public:
	Vec3 m_pos;
};

class ParticlePacket : public Packet {
public:
	ParticlePacket();
	~ParticlePacket() {}

public:
	Vec3 m_pos;
	uint16 m_particleIndex;
};

class ForcePacket : public Packet {
public:
	ForcePacket();
	~ForcePacket() {}

public:
	Vec3 m_force;
};

#pragma pack(pop)