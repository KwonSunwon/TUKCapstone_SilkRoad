#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OrientedBoxCollider.h"
#include "MeshRenderer.h"
#include "OcNode.h"
#include "OcTree.h"
#include "Network.h"
#include "Collision.h"
#include "Manifold.h"
#include "NetworkPlayer.h"
#include "Light.h"

#include "RigidBody.h"
#include "SceneManager.h"
#include "Terrain.h"

#include "UIObject.h"
#include "TextObject.h"

#include "AstarGrid.h"
#include "DifficultyManager.h"

Scene::Scene()
{

}

void Scene::Awake()
{
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	GET_SINGLE(DifficultyManager)->Update();
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}

	erase_if(m_textObjects, [](shared_ptr<TextObject>& object) { return !object->isValid(); });
	for(const shared_ptr<TextObject>& textObject : m_textObjects)
	{
		textObject->Update();
	}

}

void Scene::LateUpdate()
{
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->FinalUpdate();
	}
	m_ocTree->Update();
}

shared_ptr<Camera> Scene::GetMainCamera()
{
	if(m_cameras.empty())
		return nullptr;

	return m_cameras[0];
}

shared_ptr<Light> Scene::GetMainLight()
{
	if(m_lights.empty())
		return nullptr;

	return m_lights[0];
}

void Scene::Render()
{
	PushLightData();

	ClearRTV();

	RenderShadow();

	RenderDeferred();

	RenderLights();

	RenderFinal();

	RenderForward();
}

void Scene::ClearRTV()
{
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for(auto& light : m_lights)
	{
		if(light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	for(auto& light : m_lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->Render_Forward();

	for(auto& camera : m_cameras)
	{
		if(camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::RenderUI()
{
	for(const auto& t : m_textObjects)
		t->Render(GEngine->GetD3D11On12Device()->GetD2DDeviceContext());
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for(auto& light : m_lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}


void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if(gameObject->GetRigidBody() != nullptr)
	{
		m_collidableGameObjects.push_back(gameObject);
		m_ocTree->InsertObjectCollider(gameObject->GetCollider());
	}

	if(gameObject->GetInteractiveObject() != nullptr)
	{
		m_interactiveGameObjects.push_back(gameObject);
	}

	if(gameObject->GetCamera() != nullptr)
	{
		m_cameras.push_back(gameObject->GetCamera());
	}
	else if(gameObject->GetLight() != nullptr)
	{
		m_lights.push_back(gameObject->GetLight());
	}
	
	if(gameObject->GetNetworkObject() != nullptr)
	{
		m_networkObjects.push_back(gameObject->GetNetworkObject());
	}

	m_gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if(gameObject->GetRigidBody())
	{
		auto findIt = std::find(m_collidableGameObjects.begin(), m_collidableGameObjects.end(), gameObject);
		if(findIt != m_collidableGameObjects.end())
			m_collidableGameObjects.erase(findIt);
	}

	if(gameObject->GetCamera())
	{
		auto findIt = std::find(m_cameras.begin(), m_cameras.end(), gameObject->GetCamera());
		if(findIt != m_cameras.end())
			m_cameras.erase(findIt);
	}
	else if(gameObject->GetLight())
	{
		auto findIt = std::find(m_lights.begin(), m_lights.end(), gameObject->GetLight());
		if(findIt != m_lights.end())
			m_lights.erase(findIt);
	}

	auto findIt = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
	if(findIt != m_gameObjects.end())
		m_gameObjects.erase(findIt);

}

void Scene::AddTextObject(shared_ptr<TextObject> textObject)
{
	m_textObjects.push_back(textObject);
}

void Scene::RemoveTextObject(shared_ptr<TextObject> textObject)
{
	auto findIt = std::find(m_textObjects.begin(), m_textObjects.end(), textObject);
	if(findIt != m_textObjects.end())
		m_textObjects.erase(findIt);
}


void Scene::IntersectColliders(shared_ptr<BaseCollider> bs, shared_ptr<BaseCollider> bsDst)
{
	shared_ptr<RigidBody> rb1 = bs->GetRigidBody();
	shared_ptr<RigidBody> rb2 = bsDst->GetRigidBody();
	if(rb1->GetStatic() && rb2->GetStatic())
		return;



	shared_ptr<Vec3> normal = make_shared<Vec3>();
	shared_ptr<float> depth = make_shared<float>();

	*normal = { 0,0,0 };
	*depth = 0.f;


	//if (bs->GetColliderId() > bsDst->GetColliderId())
		//return;

	shared_ptr<Vec3> norm = make_shared<Vec3>();
	if(bs->GetGameObject()->GetTerrain())
	{
		Vec3 pos = bsDst->GetRigidBody()->GetPosition();

		shared_ptr<float> h = make_shared < float >();
		m_terrain->getHeight(pos.x, pos.z, h, norm);


		bs->SetCenter(Vec3(pos.x, *h - 100, pos.z));
	}

	if(bs->GetColliderType() == ColliderType::Sphere) {
		shared_ptr<BoundingSphere> boundingSphereSrc = dynamic_pointer_cast<SphereCollider>(bs)->GetBoundingSphere();


		if(bsDst->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

			if(!CollisionSphere(boundingSphereSrc, boundingSphereDst, normal, depth))
				return;
		}


		else if(bsDst->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

			if(!CollisionSphereBox(boundingSphereSrc, boundingOrientedBoxDst, normal, depth, false))
				return;
		}

	}


	else if(bs->GetColliderType() == ColliderType::OrientedBox) {
		shared_ptr<BoundingOrientedBox> boundingOrientedBoxSrc = dynamic_pointer_cast<OrientedBoxCollider>(bs)->GetBoundingOrientedBox();


		if(bsDst->GetColliderType() == ColliderType::OrientedBox) {
			shared_ptr<BoundingOrientedBox> boundingOrientedBoxDst = dynamic_pointer_cast<OrientedBoxCollider>(bsDst)->GetBoundingOrientedBox();

			if(!CollisionBox(boundingOrientedBoxSrc, boundingOrientedBoxDst, normal, depth))
				return;
		}


		else if(bsDst->GetColliderType() == ColliderType::Sphere) {
			shared_ptr<BoundingSphere> boundingSphereDst = dynamic_pointer_cast<SphereCollider>(bsDst)->GetBoundingSphere();

			if(!CollisionSphereBox(boundingSphereDst, boundingOrientedBoxSrc, normal, depth, true))
				return;
		}
	}


	bs->setColor(Vec4(1, 0, 0, 0), true);
	bsDst->setColor(Vec4(1, 0, 0, 0), true);
	if(bs->GetGameObject()->GetTerrain())
	{
		rb2->Move(*norm * (*depth));
		shared_ptr<Manifold> contact = make_shared<Manifold>(rb1, rb2, norm, *depth, make_shared<vector<Vec3>>(), make_shared<int>());
		FindContactPoints(bs, bsDst, contact->m_contacts, contact->m_contectCount, contact->m_normal);
		shared_ptr<Vec3> normal2 = make_shared<Vec3>();
		*normal2 = -*norm;
		shared_ptr<Manifold> contact2 = make_shared<Manifold>(rb2, rb1, normal2, *depth, contact->m_contacts, make_shared<int>());
		rb1->AddCollideEvent(contact);
		rb2->AddCollideEvent(contact2);
		m_contacts.push_back(contact);
		return;
	}

	if(rb1->GetIsBlockBody() && rb2->GetIsBlockBody()) {

		if(rb1->GetStatic()) {
			rb2->Move(*normal * (*depth));
		}

		else if(rb2->GetStatic()) {
			rb1->Move(-(*normal) * (*depth));
		}
		else {
			rb1->Move(-(*normal) * (*depth) / 2.f);
			rb2->Move(*normal * (*depth) / 2.f);
		}
	}


	shared_ptr<Manifold> contact = make_shared<Manifold>(rb1, rb2, normal, *depth, make_shared<vector<Vec3>>(), make_shared<int>());
	FindContactPoints(bs, bsDst, contact->m_contacts, contact->m_contectCount, contact->m_normal);
	shared_ptr<Vec3> normal2 = make_shared<Vec3>();
	*normal2 = -*normal;
	shared_ptr<Manifold> contact2 = make_shared<Manifold>(rb2, rb1, normal2, *depth, contact->m_contacts, make_shared<int>());
	rb1->AddCollideEvent(contact);
	rb2->AddCollideEvent(contact2);
	m_contacts.push_back(contact);



}

void Scene::testCollision()
{
	m_contacts.clear();
	for(auto& cgo : m_collidableGameObjects)
	{
		cgo->GetCollider()->setColor(Vec4(0, 0, 0, 0), false);
		cgo->GetRigidBody()->GetCollideEvent()->clear();
	}

	/*for (auto& cgo : m_collidableGameObjects)
	{
		m_ocTree->CollisionInspection(cgo->GetCollider());
	}*/

	for(int i = 0; i < m_collidableGameObjects.size(); ++i) {
		for(int j = i + 1; j < m_collidableGameObjects.size(); ++j) {
			IntersectColliders(m_collidableGameObjects[i]->GetCollider(), m_collidableGameObjects[j]->GetCollider());
		}
	}

	Resolution();

}

void Scene::PhysicsStep(int iterations)
{
	for(const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		if(gameObject->GetRigidBody())
			gameObject->GetRigidBody()->MovementStep(iterations);
	}
}

void Scene::Resolution()
{
	for(shared_ptr<Manifold> contact : m_contacts) {
		shared_ptr<RigidBody> rb1 = contact->m_rb1;
		shared_ptr<RigidBody> rb2 = contact->m_rb2;
		if(!rb1->GetIsBlockBody() || !rb2->GetIsBlockBody())
			continue;


		Vec3 normal = *contact->m_normal;


		Vec3 relativeVelocity = rb2->GetLinearVelocity() - rb1->GetLinearVelocity();
		if(relativeVelocity.Dot(normal) > 0.f) {
			continue;
		}

		float e = min(rb1->GetRestitution(), rb2->GetRestitution());
		float j = -(1.f + e) * relativeVelocity.Dot(normal);
		j /= rb1->GetInvMass() + rb2->GetInvMass();
		Vec3 impulse = (normal)*j;

		rb1->SetLinearVelocity(rb1->GetLinearVelocity() - impulse * rb1->GetInvMass());
		rb2->SetLinearVelocity(rb2->GetLinearVelocity() + impulse * rb2->GetInvMass());
	}
}

