#include"pch.h"
#include"DifficultyManager.h"
#include "Timer.h"
#include "Input.h"

void DifficultyManager::Init()
{
	m_difficulty = Difficulty::EASY;
	m_timer = 0;
}

void DifficultyManager::Update()
{
	m_timer += DELTA_TIME;

	if (m_timer > DifficultyTime::NORMAL_TIME)
	{
		m_difficulty = Difficulty::NORMAL;
	}
	if (m_timer > DifficultyTime::HARD_TIME)
	{
		m_difficulty = Difficulty::HARD;
	}
	if (m_timer > DifficultyTime::VERYHARD_TIME)
	{
		m_difficulty = Difficulty::VERYHARD;
	}

	if (GET_SINGLE(Input)->GetButtonDown(KEY_TYPE::F11))
	{
		m_isGameOver = true;
		m_gameOverTime = m_timer;
	}
}