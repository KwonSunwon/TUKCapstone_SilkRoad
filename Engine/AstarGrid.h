#pragma once

#define MOVE_DIAGONAL_COST 70
#define MOVE_STRAIGHT_COST 50

struct AstarNode
{
	Vec3 pos;
	bool isObstacle;
	std::vector<int> neighbors;
};

class AstarGrid
{
public:
	AstarGrid();
	~AstarGrid();

	void LoadGridFromFile(const std::string& fileName);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	float GetCellSize() const { return m_cellSize; }

	int GetIndex(Vec3 pos) const;
	Vec3 GetPosition(int index) const { return m_nodes[index].pos; }
	bool IsObstacle(int index) const { return m_nodes[index].isObstacle; }
	int CalculateDistanceCost(AstarNode a, AstarNode b) const;

	AstarNode GetNode(int index) const { return m_nodes[index]; }

private:
	int m_width;
	int m_height;
	float m_cellSize;
	std::vector<AstarNode> m_nodes;
};