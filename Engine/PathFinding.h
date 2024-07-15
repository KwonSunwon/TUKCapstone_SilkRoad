#pragma once

class AstarGrid;

class PathNode {
public:
	PathNode();
	~PathNode();

	int m_idx;
	bool m_isObstacle;
	PathNode* m_cameFromNode;

	int m_gCost;
	int m_hCost;
	int m_fCost;

	void CalculateFCost() { m_fCost = m_gCost + m_hCost; }

};

class PathFinding {
public:
	PathFinding();
	~PathFinding();

	std::list<PathNode> FindPath(const Vec3& start, const Vec3& end);
	std::list<PathNode> CalculatePath(PathNode* endNode);
	std::list<PathNode*> GetNeighbors(PathNode* node);

private:
	std::priority_queue<PathNode*> m_openList;
	std::priority_queue<PathNode*> m_closedList;

	shared_ptr<AstarGrid> m_astarGrid;
};