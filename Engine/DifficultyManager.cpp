#include"pch.h"
#include"DifficultyManager.h"
#include "Timer.h"
#include "Input.h"
#include "Player.h"
#include "NetworkPlayer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Network.h"
#include "UpgradeManager.h"

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

	if (INPUT->GetButtonDown(KEY_TYPE::ENTER) && m_isGameOver)
	{
		if (GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::HOST)
		{
			shared_ptr<StageChangePacket> packet = make_shared<StageChangePacket>();
			packet->m_stageIndex = 0;
			SEND(packet);
			ReturnToLobby();
		}
		else if (GET_SINGLE(NetworkManager)->GetNetworkState() == NETWORK_STATE::SINGLE)
		{
			ReturnToLobby();
		}
	}


	if (GET_SINGLE(Input)->GetButtonDown(KEY_TYPE::F11))
	{
		m_isGameOver = false;
		//m_gameOverTime = m_timer;
	}

	if (m_isGameOver)
		return;

	auto mainPlayer = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainPlayerScript();
	auto networkPlayer1 = GET_SINGLE(SceneManager)->GetActiveScene()->GetNetworkPlayers()[0];
	auto networkPlayer2 = GET_SINGLE(SceneManager)->GetActiveScene()->GetNetworkPlayers()[1];
	if (mainPlayer != nullptr && networkPlayer1 != nullptr && networkPlayer2 != nullptr)
	{
		if (mainPlayer->IsAlive() == false && networkPlayer1->IsActivated() == false && networkPlayer2->IsActivated() == false)
		{
			OutputDebugString(L"DM m_isGameOver(DM): ");
			OutputDebugString(to_wstring(GET_SINGLE(DifficultyManager)->GetIsGameOver()).c_str());
			OutputDebugString(L"\n");
			m_isGameOver = true;
			m_gameOverTime = m_timer;
		}
	}
}

void DifficultyManager::ReturnToLobby()
{
	GET_SINGLE(UpgradeManager)->AddGold(m_gameOverTime * 0.8);
	for (int i = 0; i < 17; ++i)
	{
		GET_SINGLE(UpgradeManager)->SetItemLevel(i, 0);
	}
	
	m_difficulty = Difficulty::EASY;
	m_timer = 0;
	m_isGameOver = false;
	m_gameOverTime = 0;

	GET_SINGLE(SceneManager)->StartStage(0);
}