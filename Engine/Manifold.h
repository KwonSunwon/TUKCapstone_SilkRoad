#pragma once

class RigidBody;
class Manifold
{
public:
	Manifold(shared_ptr<RigidBody> rb1, shared_ptr<RigidBody> rb2, 
		shared_ptr<Vec3> normal, float depth, shared_ptr<vector<Vec3>> contacts, 
		shared_ptr<int> contectCount)
	{
		m_rb1 = rb1;
		m_rb2 = rb2;
		m_normal = normal;
		m_depth = depth;
		m_contacts = contacts;
		m_contectCount = contectCount;
	}

public:
	shared_ptr<RigidBody> m_rb1;
	shared_ptr<RigidBody> m_rb2;
	shared_ptr<Vec3> m_normal;
	float m_depth;
	shared_ptr<vector<Vec3>> m_contacts;
	shared_ptr<int> m_contectCount;

};

