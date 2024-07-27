#include "pch.h"
#include "SoundManager.h"
#include<fmod.hpp>
void SoundManager::Init()
{
    FMOD::System_Create(&m_soundSystem);
    m_soundSystem->init(256, FMOD_INIT_NORMAL, 0);


    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Space.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_SPACE]);
    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Desert.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_DESERT]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Weapon_Assult_Fire.wav", FMOD_DEFAULT, 0, &m_sound[WEAPON_ASSULT_FIRE]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Hit_Enemy.wav", FMOD_DEFAULT, 0, &m_sound[ENV_HIT_ENEMY]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Eat_Item.wav", FMOD_DEFAULT, 0, &m_sound[ENV_EAT_ITEM]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Skill_Tanker.wav", FMOD_DEFAULT, 0, &m_sound[SKILL_TANKER]);



}

void SoundManager::soundPlay(int _type)
{
    m_soundSystem->playSound(m_sound[_type], 0, false, &m_channel[_type]);

    if (_type == WEAPON_ASSULT_FIRE)
    {
        m_channel[_type]->setVolume(0.5f); // 볼륨을 10%로 설정
    }
    if (_type == BGM_SPACE)
    {
        m_channel[_type]->setVolume(0.7f); // 볼륨을 10%로 설정
    }

    m_soundSystem->update();
}

void SoundManager::soundStop(int _type)
{
    m_channel[_type]->stop();
    m_soundSystem->update();
}




