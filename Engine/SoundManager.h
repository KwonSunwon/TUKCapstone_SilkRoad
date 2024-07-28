#pragma once
#include<fmod.hpp>

enum Sounds
{
    BGM_SPACE,
    BGM_DESERT,
    BGM_SNOWFIELD,
    BGM_CORRUPTION,
    WEAPON_ASSULT_FIRE,
    ENV_HIT_ENEMY,
    ENV_EAT_ITEM,
    SKILL_TANKER,
    SKILL_HEALER,
    SOUND_TYPE_COUNT
};

class SoundManager
{
	DECLARE_SINGLE(SoundManager);

private:
    FMOD::System* m_soundSystem;
    FMOD::Sound* m_sound[SOUND_TYPE_COUNT];
    FMOD::Channel* m_channel[SOUND_TYPE_COUNT];

public:
    void Init();
    void soundPlay(int enumSounds);
    void soundStop(int enumSounds);
	
};
