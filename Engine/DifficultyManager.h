#pragma once

enum Difficulty
{
	EASY = 0,
	NORMAL = 1,
	HARD = 2,
	VERYHARD = 3
};

enum DifficultyTime
{
	EASY_TIME = 0,
	NORMAL_TIME = 5,
	HARD_TIME = 10,
	VERYHARD_TIME = 15
};

class DifficultyManager
{
	DECLARE_SINGLE(DifficultyManager);

public:
	void Init();
	void Update();
	float GetTimer() { return m_timer; }
	Difficulty GetDifficulty() { return m_difficulty; }
	bool GetIsGameOver() { return m_isGameOver; }
	void SetIsGameOver(bool isGameOver) { m_isGameOver = isGameOver; }
	float GetGameOverTime() { return m_gameOverTime; }

	void ReturnToLobby();

private:
	Difficulty m_difficulty = Difficulty::EASY;
	float m_timer = 0;

	bool m_isGameOver = false;
	float m_gameOverTime = 0;
};