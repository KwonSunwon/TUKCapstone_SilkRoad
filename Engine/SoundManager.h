#pragma once
#include<fmod.hpp>

enum Sounds
{
    BGM,
    BUTTON_CLICK,
};

class SoundManager
{
	DECLARE_SINGLE(SoundManager);

private:
    FMOD::System* m_soundSystem;
    FMOD::Sound* m_sound[5];
    FMOD::Channel* m_channel;


public:
    void Init();
    void soundPlay(int enumSounds);
    void soundStop(int enumSounds);
	
};
