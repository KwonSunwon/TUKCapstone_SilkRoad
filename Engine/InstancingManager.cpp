#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<uint64, vector<shared_ptr<GameObject>>> cache;

	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		if (gameObject->IsActive() == false)
			continue;
		const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject);
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		if (vec.size() == 1)
		{
			vec[0]->GetMeshRenderer()->Render();
		}
		else
		{
			const uint64 instanceId = pair.first;

			for (const shared_ptr<GameObject>& gameObject : vec)
			{
				InstancingParams params;
				params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
				params.matWV = params.matWorld * Camera::S_MatView;
				params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

				AddParam(instanceId, params);
			}

			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceId];
			vec[0]->GetMeshRenderer()->Render(buffer);
		}
	}
}

void InstancingManager::ClearBuffer()
{
	for (auto& pair : m_buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second;
		buffer->Clear();
	}
}

void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data)
{
	if (m_buffers.find(instanceId) == m_buffers.end())
		m_buffers[instanceId] = make_shared<InstancingBuffer>();

	m_buffers[instanceId]->AddData(data);
}