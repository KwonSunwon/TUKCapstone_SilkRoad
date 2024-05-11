#include "pch.h"
#include "Item.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Timer.h"


void Item::Awake()
{
	SetMonovihaviourName("Item");
	m_startHeight = GetTransform()->GetLocalPosition().y;
	

}
void Item::Update()
{
	FloatingItem();
	RotateItem();
}

void Item::LateUpdate()
{
	
}

void Item::FloatingItem()
{
	m_radian += m_frequency * DELTA_TIME * PI * 2;
	float nextHeight = m_startHeight + m_amplitude * sin(m_radian);
	Vec3 nextPos = GetRigidBody()->GetPosition();
	nextPos.y = nextHeight;
	GetRigidBody()->MoveTo(nextPos);
}
void Item::RotateItem()
{
	GetTransform()->SetLocalRotation(Vec3(0.f, m_radian, 0.f));
}

