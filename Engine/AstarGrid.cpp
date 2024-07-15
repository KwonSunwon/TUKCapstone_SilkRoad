#include "pch.h"
#include "AstarGrid.h"

AstarGrid::AstarGrid()
{
    m_width = 100;
    m_height = 100;
	m_cellSize = 500.f;
}

AstarGrid::~AstarGrid()
{
}

void AstarGrid::LoadGridFromFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open())
        return;

    int numNodes;
    file.read(reinterpret_cast<char*>(&numNodes), sizeof(numNodes));

    m_nodes.resize(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        file.read(reinterpret_cast<char*>(&m_nodes[i].pos.x), sizeof(m_nodes[i].pos.x));
        file.read(reinterpret_cast<char*>(&m_nodes[i].pos.y), sizeof(m_nodes[i].pos.y));
        file.read(reinterpret_cast<char*>(&m_nodes[i].pos.z), sizeof(m_nodes[i].pos.z));
        file.read(reinterpret_cast<char*>(&m_nodes[i].isObstacle), sizeof(m_nodes[i].isObstacle));

        int numNeighbors;
        file.read(reinterpret_cast<char*>(&numNeighbors), sizeof(numNeighbors));
        m_nodes[i].neighbors.resize(numNeighbors);
        for (int j = 0; j < numNeighbors; ++j)
        {
            int neighbor;
            file.read(reinterpret_cast<char*>(&neighbor), sizeof(neighbor));
            m_nodes[i].neighbors[j] = neighbor;
        }
    }

    file.close();


}

int AstarGrid::GetIndex(Vec3 pos) const
{
	int x = static_cast<int>(pos.x / m_cellSize);
	int z = static_cast<int>(pos.z / m_cellSize);

	return x * m_height + z;
}

int AstarGrid::CalculateDistanceCost(AstarNode a, AstarNode b) const
{
	int xDistance = abs(static_cast<int>(a.pos.x / m_cellSize) - static_cast<int>(b.pos.x / m_cellSize));
	int zDistance = abs(static_cast<int>(a.pos.z / m_cellSize) - static_cast<int>(b.pos.z/m_cellSize));
	int remaining = abs(xDistance - zDistance);

	return MOVE_DIAGONAL_COST * min(xDistance, zDistance) + MOVE_STRAIGHT_COST * remaining;
}