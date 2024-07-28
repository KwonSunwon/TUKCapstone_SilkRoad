#include "pch.h"
#include "SoundManager.h"
#include<fmod.hpp>
void SoundManager::Init()
{
    FMOD::System_Create(&m_soundSystem);
    m_soundSystem->init(256, FMOD_INIT_NORMAL, 0);


    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Space.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_SPACE]);
    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Desert.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_DESERT]);
    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Snowfield.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_SNOWFIELD]);
    m_soundSystem->createSound("..\\Resources\\Sound\\BGM_Corruption.mp3", FMOD_LOOP_NORMAL, 0, &m_sound[BGM_CORRUPTION]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Weapon_Assult_Fire.wav", FMOD_DEFAULT, 0, &m_sound[WEAPON_ASSULT_FIRE]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Hit_Enemy.wav", FMOD_DEFAULT, 0, &m_sound[ENV_HIT_ENEMY]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Eat_Item.wav", FMOD_DEFAULT, 0, &m_sound[ENV_EAT_ITEM]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Thunder.wav", FMOD_DEFAULT, 0, &m_sound[ENV_THUNDER]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Gate_On.wav", FMOD_DEFAULT, 0, &m_sound[ENV_GATE_ON]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Fail.wav", FMOD_DEFAULT, 0, &m_sound[ENV_FAIL]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Warp.wav", FMOD_DEFAULT, 0, &m_sound[ENV_WARP]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_FootStep.wav", FMOD_DEFAULT, 0, &m_sound[ENV_FOORSTEP]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Env_Hit_By_Enemy.wav", FMOD_DEFAULT, 0, &m_sound[ENV_HIT_BY_ENEMY]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Skill_Tanker.wav", FMOD_DEFAULT, 0, &m_sound[SKILL_TANKER]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Skill_Healer.wav", FMOD_DEFAULT, 0, &m_sound[SKILL_HEALER]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Skill_Dealer.wav", FMOD_DEFAULT, 0, &m_sound[SKILL_DEALER]);
    m_soundSystem->createSound("..\\Resources\\Sound\\Skill_Launcher.wav", FMOD_DEFAULT, 0, &m_sound[SKILL_LAUNCHER]);



}

void SoundManager::soundPlay(int _type)
{
    m_soundSystem->playSound(m_sound[_type], 0, false, &m_channel[_type]);

    if (_type == WEAPON_ASSULT_FIRE)
    {
        m_channel[_type]->setVolume(0.5f); // 볼륨을 10%로 설정
    }
    else if (_type == BGM_SPACE)
    {
        m_channel[_type]->setVolume(0.7f); // 볼륨을 10%로 설정
    }
    else if (_type == ENV_EAT_ITEM)
    {
        m_channel[_type]->setVolume(0.4f); // 볼륨을 10%로 설정
    }
    else if (_type == SKILL_LAUNCHER)
    {
        m_channel[_type]->setVolume(0.3f); // 볼륨을 10%로 설정
    }
    else if (_type == ENV_THUNDER)
    {
        m_channel[_type]->setVolume(0.7f); // 볼륨을 10%로 설정
    }
    else if (_type == ENV_WARP)
    {
        m_channel[_type]->setVolume(0.7f); // 볼륨을 10%로 설정
    }
    else if (_type == ENV_GATE_ON)
    {
        m_channel[_type]->setVolume(0.6f); // 볼륨을 10%로 설정
    }

    m_soundSystem->update();
}

void SoundManager::soundStop(int _type)
{
    m_channel[_type]->stop();
    m_soundSystem->update();
}




