#include "pch.h"
#include "AstarGrid.h"
#include "PathFinding.h"
#include "Scene.h"
#include "SceneManager.h"

PathNode::PathNode()
{
	m_idx = -1;
	m_isObstacle = false;
	m_cameFromNode = nullptr;

	m_gCost = 0;
	m_hCost = 0;
	m_fCost = INT_MAX;
}

PathNode::~PathNode()
{

}

PathFinding::PathFinding()
{

}

PathFinding::~PathFinding()
{
}

struct CompareFCost {
    bool operator()(const PathNode* a, const PathNode* b) const {
        return a->m_fCost > b->m_fCost;
    }
};

std::list<PathNode> PathFinding::FindPath(const Vec3& start, const Vec3& end)
{
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto astarGrid = scene->GetAstarGrid();

    int startIdx = astarGrid->GetIndex(start);
    int endIdx = astarGrid->GetIndex(end);

    std::priority_queue<PathNode*, std::vector<PathNode*>, CompareFCost> openList;
    std::unordered_map<int, PathNode*> allNodes;
    std::unordered_map<int, bool> closedList;

    PathNode* startNode = new PathNode();
    startNode->m_idx = startIdx;
    startNode->m_isObstacle = astarGrid->IsObstacle(startIdx);
    startNode->m_gCost = 0;
    startNode->m_hCost = astarGrid->CalculateDistanceCost(astarGrid->GetNode(startIdx), astarGrid->GetNode(endIdx));
    startNode->CalculateFCost();

    openList.push(startNode);
    allNodes[startIdx] = startNode;

    while (!openList.empty()) {
        PathNode* currentNode = openList.top();
        openList.pop();

        if (currentNode->m_idx == endIdx) {
            std::list<PathNode> path = CalculatePath(currentNode);
            for (auto node : allNodes) {
                delete node.second;
            }
            return path;
        }

        closedList[currentNode->m_idx] = true;

        for (PathNode* neighbor : GetNeighbors(currentNode)) {
            if (closedList[neighbor->m_idx]) {
                delete neighbor;
                continue;
            }

            int tentativeGCost = currentNode->m_gCost + astarGrid->CalculateDistanceCost(astarGrid->GetNode(currentNode->m_idx), astarGrid->GetNode(neighbor->m_idx));
            if (tentativeGCost <= neighbor->m_gCost) {
                neighbor->m_cameFromNode = currentNode;
                neighbor->m_gCost = tentativeGCost;
                neighbor->m_hCost = astarGrid->CalculateDistanceCost(astarGrid->GetNode(neighbor->m_idx), astarGrid->GetNode(endIdx));
                neighbor->CalculateFCost();

                if (allNodes.find(neighbor->m_idx) == allNodes.end()) {
                    openList.push(neighbor);
                    allNodes[neighbor->m_idx] = neighbor;
                }
            }
        }
    }

    for (auto node : allNodes) {
        delete node.second;
    }

    return std::list<PathNode>(); // 경로를 찾지 못한 경우 빈 리스트 반환
}

std::list<PathNode> PathFinding::CalculatePath(PathNode* endNode)
{
	std::list<PathNode> path;
	PathNode* currentNode = endNode;
	while (currentNode != nullptr)
	{
		path.push_front(*currentNode);
		currentNode = currentNode->m_cameFromNode;
	}

	return path;
}

std::list<PathNode*> PathFinding::GetNeighbors(PathNode* node)
{
	std::list<PathNode*> neighbors;
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	shared_ptr<AstarGrid> astarGrid = scene->GetAstarGrid();

	for (int neighborIdx : astarGrid->GetNode(node->m_idx).neighbors)
	{
		PathNode* neighbor = new PathNode();
		neighbor->m_idx = neighborIdx;
		neighbor->m_isObstacle = astarGrid->IsObstacle(neighborIdx);
        neighbor->m_gCost = node->m_gCost + MOVE_STRAIGHT_COST;
		neighbors.push_back(neighbor);
	}

	return neighbors;
}

void RemoveNodeFromList(std::priority_queue<PathNode*>& list, PathNode* node)
{
	std::vector<PathNode*> temp;
	while (list.size() > 0)
	{
		PathNode* currentNode = list.top();
		list.pop();
		if (currentNode != node)
		{
			temp.push_back(currentNode);
		}
	}

	for (PathNode* node : temp)
	{
		list.push(node);
	}
}