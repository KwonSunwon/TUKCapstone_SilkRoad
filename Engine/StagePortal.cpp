#include "pch.h"
#include "StagePortal.h"
#include "TextObject.h"
#include "UIObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"
#include "Transform.h"
#include "Enemy.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Packet.h"
#include "Network.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist(0.f, 6.28f);

void StagePortal::Update()
{

    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto lastEnemies = scene->GetLastWaveEnemies();
    if (m_isActivated == false)
        return;
    if (m_timer > 0.f)
    {
        m_timer -= GET_SINGLE(Timer)->GetDeltaTime();
        m_spawnTimer += GET_SINGLE(Timer)->GetDeltaTime();
        if (m_spawnTimer >= 60 / static_cast<float>(lastEnemies.size()) && m_spawnIdx < lastEnemies.size())
        {
            // 적 스폰
            Vec3 enemyPos = GetTransform()->GetLocalPosition();
            float rot = dist(gen);
            enemyPos.x += cos(rot) * 10000.f;
            enemyPos.z += sin(rot) * 10000.f;
            enemyPos.y = 1000.f;
            lastEnemies[m_spawnIdx]->SetChaseRange(15000.f);
            lastEnemies[m_spawnIdx]->GetRigidBody()->MoveTo(enemyPos);
            lastEnemies[m_spawnIdx]->GetRigidBody()->SetStatic(false);
            ++m_spawnIdx;
            m_spawnTimer = 0.f;
        }

    }
    else if(!m_spawnP){
        m_spawnP = true;
        GET_SINGLE(SceneManager)->GetActiveScene()->SpawnParticle(GetTransform()->GetWorldPosition() + Vec3(50.f, 350.f, 0.f), ParticleType::PARTICLE_GATE_COMP);
    }

}

void StagePortal::PrintInteractiveText()
{
    wstring text{};

    if (m_isActivated)
    {
        if (m_timer > 0.f)
            text = (L"게이트 활성화 중 ...");
        else
            text = (L"다음 스테이지로 이동");
    }
    else
        text = (L"게이트 활성화");


    GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetText(text);
    GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetPosition(Vec2(0.f, -30.f));
    GET_SINGLE(SceneManager)->GetActiveScene()->GetInteractiveObjectText()->SetVisible(true);
}

void StagePortal::InteractiveFunction()
{
    if (m_isActivated == false)
    {

        
        //GET_SINGLE(SceneManager)->GetActiveScene()->SpawnParticle(GetTransform()->GetWorldPosition()+Vec3(0.f,2000.f,0.f), ParticleType::PARTICLE_BEAM);
        GET_SINGLE(SceneManager)->GetActiveScene()->SpawnParticle(GetTransform()->GetWorldPosition()+Vec3(50.f,350.f,0.f), ParticleType::PARTICLE_GATE);
        if (!m_isPacketProcess) {
            shared_ptr<PortalOnPacket> packet = make_shared<PortalOnPacket>();
            SEND(packet);
        }

        m_isActivated = true;
        m_timer = 60.f;
        m_spawnIdx = 0;
        m_spawnTimer = 0.f;

        vector<shared_ptr<GameObject>>gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetCollidableGameObjects();

        for (auto& gameObject : gameObjects)
        {
            if (gameObject->GetMonobehaviour("Enemy"))
            {
                shared_ptr<MonoBehaviour> scriptE = gameObject->GetMonobehaviour("Enemy");
                scriptE->GetRigidBody()->SetStatic(true);
                scriptE->GetRigidBody()->MoveTo(Vec3(-1000000.f, 1500.f, 0.f));
            }
        }
    }
    else
    {
        if (m_timer <= 0) {
            shared_ptr<StageChangePacket> packet = make_shared<StageChangePacket>();
            SEND(packet);
            GET_SINGLE(SceneManager)->StartNextStage();
        }
    }
}
