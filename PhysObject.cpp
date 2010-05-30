#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"
#include "PhysWorld.h"
#include "ObjectReplicator.h"

namespace Sarona
{
	PhysObject::PhysObject(PhysWorld* world, btDynamicsWorld* dynworld, const btTransform& initialpos)
		:	m_dynamicsWorld(dynworld)
		,	m_world(world)
		,	m_body("cube")
		,	m_shape(NULL)
		,	m_mass(1)
		,	m_dirty(true)
		,	ZCom_ReplicatorBasic(new ZCom_ReplicatorSetup( // Replicator setup goes here
			ZCOM_REPFLAG_RARELYCHANGED|ZCOM_REPFLAG_MOSTRECENT|ZCOM_REPFLAG_SETUPAUTODELETE, ZCOM_REPRULE_AUTH_2_ALL
		))
	{
		m_mesh[0] = m_texture[0] = NULL;
		m_flags |= ZCOM_REPLICATOR_INITIALIZED;

		m_zcomNode.reset(new ZCom_Node());

		ZCom_ReplicatorSetup setup(/*flags*/ ZCOM_REPFLAG_UNRELIABLE|ZCOM_REPFLAG_MOSTRECENT, /*rules*/ ZCOM_REPRULE_AUTH_2_ALL, /*intercept_id*/ 0, /*min_delay*/ 0, /*max_delay*/ 50);
		ObjectReplicator* replicator = new ObjectReplicator(setup.Duplicate(), this);

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(2);
			m_zcomNode -> addReplicator(replicator, true);
			m_zcomNode -> addReplicator(this, false); // autodelete = false, pretty important when passing 'this' ;)
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_zcomNode -> setEventInterceptor(this);
		m_zcomNode -> registerNodeDynamic(BaseWorld::m_objectId, m_world);

		m_motionstate.reset(new PhysObjectMotionState(initialpos, replicator));


		recreateBody();
	}

	PhysObject::~PhysObject(void)
	{
		if(m_motionstate)
			m_motionstate->reset(); // don't reference the replicator any longer

		m_dynamicsWorld->removeRigidBody(get_pointer(m_rigidbody));
	}

	void PhysObject::setMass(btScalar kilos)
	{
		m_mass = kilos;
		recreateBody();
	}

	void PhysObject::setTexture(const std::string & texture)
	{
		strncpy_s(m_texture, texture.c_str(), 64);
		m_dirty = true;
	}

	/*void PhysObject::setPosition(const btVector3 & pos)
	{
		m_rigidbody->getCenterOfMassPosition() = pos;
	}*/
	void PhysObject::setBody(const std::string& body)
	{
		// body property is not replicated so no need to set the object dirty.
		m_body = body;
		recreateBody();
	}
	void PhysObject::setMesh(const std::string& mesh)
	{
		strncpy_s(m_mesh, mesh.c_str(), 64);
		m_dirty = true;
	}

	void PhysObject::recreateBody()
	{
		if(m_motionstate)
			m_motionstate->setBody(NULL);
		if(m_rigidbody)
			m_dynamicsWorld->removeRigidBody(get_pointer(m_rigidbody));

        btVector3 inertia(0,0,0);
        
		m_shape = m_world->getShape(m_body);

		if(m_mass > 0)
		{
			m_shape->calculateLocalInertia(m_mass,inertia);
		}

		btRigidBody::btRigidBodyConstructionInfo CI(m_mass,get_pointer(m_motionstate),get_pointer(m_shape),inertia);
		m_rigidbody.reset(new btRigidBody(CI));
		
		m_motionstate->setBody(get_pointer(m_rigidbody));
		m_dynamicsWorld->addRigidBody(get_pointer(m_rigidbody));
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

	
	bool PhysObject::checkState ()
	{
		if(m_dirty)
		{
			m_dirty = false;
			return true;
		}
		return false;
	}
	void PhysObject::packData (ZCom_BitStream *_stream)
	{
		_stream->addString(m_mesh);
		_stream->addString(m_texture);
	}
	void PhysObject::unpackData (ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent)
	{
		// We don't receive, only send.
	}
	void* PhysObject::peekData(){return NULL;};
	void PhysObject::clearPeekData(){};
	void PhysObject::Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed){};
}
