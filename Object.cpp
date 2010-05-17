#include "StdAfx.h"
#include "Object.h"

namespace Sarona
{

	Object::Object(IrrlichtDevice* dev, btDynamicsWorld* world, scene::ISceneNode* node, btCollisionShape* shape, const btTransform& initialpos)
		:	m_device(dev)
		,	m_world(world)
		,	m_scenenode(node)
		,	m_shape(shape)
	{
		m_motionstate.reset(new ObjectMotionState(initialpos, m_scenenode));
		btRigidBody::btRigidBodyConstructionInfo CI(0,get_pointer(m_motionstate),get_pointer(m_shape),btVector3(0,0,0));

		m_rigidbody.reset(new btRigidBody(CI));
		m_world->addRigidBody(get_pointer(m_rigidbody));
	}

	Object::~Object(void)
	{
		m_world->removeRigidBody(get_pointer(m_rigidbody));
	}

	void Object::setMass(btScalar kilos)
	{
		m_world->removeRigidBody(get_pointer(m_rigidbody));

        btVector3 inertia(0,0,0);
        
		if(kilos > 0)
		{
			m_shape->calculateLocalInertia(kilos,inertia);
		}

		btRigidBody::btRigidBodyConstructionInfo CI(kilos,get_pointer(m_motionstate),get_pointer(m_shape),inertia);
		m_rigidbody.reset(new btRigidBody(CI));
		
		m_world->addRigidBody(get_pointer(m_rigidbody));
	}
}
