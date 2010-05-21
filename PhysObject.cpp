#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"
#include "ObjectReplicator.h"

namespace Sarona
{

	PhysObject::PhysObject(ZCom_Control* control, btDynamicsWorld* world, btCollisionShape* shape, const btTransform& initialpos)
		:	m_world(world)
		,	m_shape(shape)
	{
		m_zcomNode.reset(new ZCom_Node());

		ZCom_ReplicatorSetup setup(/*flags*/ ZCOM_REPFLAG_UNRELIABLE   , /*rules*/ ZCOM_REPRULE_AUTH_2_ALL, /*intercept_id*/ 0, /*min_delay*/ 0, /*max_delay*/ 50);
		ObjectReplicator* replicator = new ObjectReplicator(setup.Duplicate(), this);

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(1);
			m_zcomNode -> addReplicator(replicator, true);
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_zcomNode -> setEventInterceptor(this);
		m_zcomNode -> registerNodeDynamic(BaseWorld::m_objectId, control);


		m_motionstate.reset(new PhysObjectMotionState(initialpos, replicator));
		btRigidBody::btRigidBodyConstructionInfo CI(0,get_pointer(m_motionstate),get_pointer(m_shape),btVector3(0,0,0));

		m_rigidbody.reset(new btRigidBody(CI));
		m_world->addRigidBody(get_pointer(m_rigidbody));
	}

	PhysObject::~PhysObject(void)
	{
		m_motionstate->reset(); // don't reference the replicator any longer

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


	bool PhysObject::recUserEvent(ZCom_Node *_node, ZCom_ConnID _from, 
					eZCom_NodeRole _remoterole, ZCom_BitStream &_data, 
					zU32 _estimated_time_sent)
	{
		return false;
	}
	                          
	bool PhysObject::recInit(ZCom_Node *_node, ZCom_ConnID _from,
			   eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool PhysObject::recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from, 
					  eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool PhysObject::recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
				  eZCom_NodeRole _remoterole)
	{
		return false;
	}
	                        
	bool PhysObject::recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid, 
					   ZCom_BitStream &_request)
	{
		return false;
	}
	                             
	bool PhysObject::recFileData(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}
	                     
	bool PhysObject::recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) 
	{
		return false;
	}
	                           
	bool PhysObject::recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}
}
