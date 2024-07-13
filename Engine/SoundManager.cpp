#include "pch.h"
#include "SoundManager.h"
#include<fmod.hpp>
void SoundManager::Init()
{
    FMOD::System_Create(&m_soundSystem);
    m_soundSystem->init(32, FMOD_INIT_NORMAL, 0);

    m_soundSystem->createSound("..\\Resources\\Sound\\bgm.ogg", FMOD_LOOP_NORMAL, 0, &m_sound[Sounds::BGM]);
    m_soundSystem->createSound("..\\Resources\\Sound\\button_click.wav", FMOD_DEFAULT, 0, &m_sound[Sounds::BUTTON_CLICK]);

}

void SoundManager::soundPlay(int _type)
{
    m_soundSystem->playSound(m_sound[_type], 0, false, &m_channel);
}

void SoundManager::soundStop(int _type)
{
    m_channel->stop();
}



