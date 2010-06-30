#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"
#include "PhysWorld.h"
#include "ObjectReplicator.h"
#include "PacketDef.h"

namespace Sarona
{
	PhysObject::PhysObject(PhysWorld* world, btDynamicsWorld* dynworld, const btTransform& initialpos)
		:	m_dynamicsWorld(dynworld)
		,	m_world(world)
		,	m_body("cube")
		,	m_shape(NULL)
		,	m_mass(0)
		,	m_meshScale(1)
		,	m_bodyScale(1)
		,	m_dirty(true)
		,	m_deleteme(false)
	{
		m_zcomNode.reset(new ZCom_Node());
		m_zcomNode->setUserData(this);

		m_replicator = new ObjectReplicator(
			new ZCom_ReplicatorSetup(ZCOM_REPFLAG_UNRELIABLE|ZCOM_REPFLAG_MOSTRECENT|ZCOM_REPFLAG_SETUPAUTODELETE, ZCOM_REPRULE_AUTH_2_ALL), this);

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(1);
			m_zcomNode -> addReplicator(m_replicator, true);
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_motionstate.reset(new PhysObjectMotionState(initialpos, m_replicator));
		
		recreateBody();

		m_zcomNode -> setEventInterceptor(this);
		m_zcomNode -> registerNodeDynamic(TypeRegistry::m_objectId, m_world);
		// We want to be informed about connecting clients so that we can send our current state to them directly.
		m_zcomNode -> setEventNotification(true, false); 
	}

	PhysObject::~PhysObject(void)
	{
		if(m_motionstate)
			m_motionstate->reset(); // don't reference the replicator any longer

		m_zcomNode -> setEventInterceptor(NULL);
//		m_zcomNode -> unregisterNode();
		m_zcomNode.reset();
		//m_replicator.reset();

		m_dynamicsWorld->removeRigidBody(get_pointer(m_rigidbody));
	}

	void PhysObject::setMass(btScalar kilos)
	{
		m_mass = kilos;
		recreateBody();
	}

	void PhysObject::setTexture(const std::string & texture)
	{
		if(m_texture == texture)
			return;

		m_texture = texture;
		m_dirty = true;
	}

	void PhysObject::setBody(const std::string& body)
	{
		// body property is not replicated so no need to set the object dirty.
		m_body = body;
		recreateBody();
	}
	void PhysObject::setMesh(const std::string& mesh)
	{
		if(m_mesh== mesh)
			return;

		m_mesh = mesh;
		m_dirty = true;
	}
	void PhysObject::setMeshScale(float in)
	{
		m_meshScale = in;
		m_dirty = true;
	}
	void PhysObject::setBodyScale(float in)
	{
		m_bodyScale = in;
		recreateBody();
	}
	ZCom_NodeID PhysObject::getNetworkId()
	{
		if(m_zcomNode)
			return m_zcomNode->getNetworkID();
		return 0;
	}
	void PhysObject::kill() 
	{
		m_deleteme = true;
	}
	bool PhysObject::isZombie()
	{
		return m_deleteme;
	}
	void PhysObject::recreateBody()
	{
		if(m_motionstate)
			m_motionstate->setBody(NULL);
		if(m_rigidbody)
			m_dynamicsWorld->removeRigidBody(get_pointer(m_rigidbody));

        btVector3 inertia(0,0,0);
        
		// Get static body if zero mass
		m_shape = m_world->getShape(m_body, m_mass <= 0);
		m_shape->setLocalScaling(btVector3(
			m_bodyScale, m_bodyScale, m_bodyScale
			));

		if(m_mass > 0)
		{
			m_shape->calculateLocalInertia(m_mass,inertia);
		}

		btRigidBody::btRigidBodyConstructionInfo CI(m_mass,get_pointer(m_motionstate),get_pointer(m_shape),inertia);
		m_rigidbody.reset(new btRigidBody(CI));
		
		m_motionstate->setBody(get_pointer(m_rigidbody));
		m_dynamicsWorld->addRigidBody(get_pointer(m_rigidbody));
	}
	void PhysObject::push(const btVector3& force)
	{
		if(m_rigidbody)
			m_rigidbody->applyImpulse(force, btVector3(0,0,0));
	}

	void PhysObject::sendUpdate()
	{
		// Send status update to all peers
		if(!m_dirty)
			return;

		ZCom_BitStream stream;

		Protocol::NodeStateUpdate state;
		state.mesh = m_mesh;
		state.texture = m_texture;
		state.mesh_scale = m_meshScale;

		state.write(stream);

		m_zcomNode->sendEvent(eZCom_ReliableUnordered, ZCOM_REPRULE_AUTH_2_ALL, stream.Duplicate());

		m_dirty = false;
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
		// New client has connected
		// Let's send our current transform to them 
		// (important in case of static/nonmoving objects that don't generate position updates often (if at all))
		btTransform current;
		m_motionstate->getWorldTransform(current);

		btVector3 velocity(0,0,0);
		btVector4 omega(0,0,0,0);
		m_replicator->Input(current.getOrigin(), current.getRotation(), velocity, omega);

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
