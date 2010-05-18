#include "StdAfx.h"
#include "PhysObject.h"

namespace Sarona
{

	PhysObject::PhysObject(IrrlichtDevice* dev, scene::ISceneNode* node, btDynamicsWorld* world, btCollisionShape* shape, const btTransform& initialpos)
		:	BaseObject(dev, node)
		,	m_world(world)
		,	m_shape(shape)
	{
		m_motionstate.reset(new PhysObjectMotionState(initialpos, m_scenenode));
		btRigidBody::btRigidBodyConstructionInfo CI(0,get_pointer(m_motionstate),get_pointer(m_shape),btVector3(0,0,0));

		m_rigidbody.reset(new btRigidBody(CI));
		m_world->addRigidBody(get_pointer(m_rigidbody));
	}

	PhysObject::~PhysObject(void)
	{
		m_world->removeRigidBody(get_pointer(m_rigidbody));
	}

	void PhysObject::setMass(btScalar kilos)
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
