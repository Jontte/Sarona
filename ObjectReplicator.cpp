#include "StdAfx.h"
#include "ObjectReplicator.h"
#include "NetObject.h"
#include "PhysObject.h"

namespace Sarona
{

	ObjectReplicator::ObjectReplicator(ZCom_ReplicatorSetup* setup, PhysObject* phys) 
		: ZCom_ReplicatorBasic(setup)
		, m_phys(phys)
		, m_net(NULL)
	{
		init_replicator();
	}
	ObjectReplicator::ObjectReplicator(ZCom_ReplicatorSetup* setup, NetObject* net) 
		: ZCom_ReplicatorBasic(setup)
		, m_net(net)
		, m_phys(NULL)
	{
		init_replicator();
	}

	void ObjectReplicator::init_replicator()
	{
		m_flags |= ZCOM_REPLICATOR_INITIALIZED;
		m_dirty = false;
	}
	
	void ObjectReplicator::Input(float position[3])
	{
		// mark dirty if position change significant

		btVector3 temp(position[0]-m_position[0],position[1]-m_position[1],position[2]-m_position[2]);

		if(temp.length2() > (0.125*0.125))
		{
			m_dirty = true;

			m_position[0] = position[0];
			m_position[1] = position[1];
			m_position[2] = position[2];
		}
	}
	
	bool ObjectReplicator::checkState()
	{
		// should we update?
		if(m_phys && m_dirty)
		{
			m_dirty = false;
			return true;
		}
		return false;
	}
	void ObjectReplicator::packData(ZCom_BitStream *_stream)
	{
		// what to do when to send update
		_stream->addFloat(m_position[0], 8);
		_stream->addFloat(m_position[1], 8);
		_stream->addFloat(m_position[2], 8);
	}
	void ObjectReplicator::unpackData(ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent)
	{
		// what to do when to receive update 
		m_position[0] = _stream->getFloat(8);
		m_position[1] = _stream->getFloat(8);
		m_position[2] = _stream->getFloat(8);

		// broadcast object position back to irrlicht.
		if(m_net) // m_net should be there always if we get here
		{
			m_net->SetPosition(core::vector3df(m_position[0], m_position[1], m_position[2]));
		}
	}

	ObjectReplicator::~ObjectReplicator(void)
	{

	}

	void ObjectReplicator::clearPeekData ()
	{
	
	}
	void* ObjectReplicator::peekData()
	{
		return NULL;
	}
	
	void ObjectReplicator::Process (eZCom_NodeRole _localrole, zU32 _simulation_time_passed)
	{
	
	}
	/*
	void ObjectReplicator::onConnectionAdded (ZCom_ConnID _cid, eZCom_NodeRole _remoterole)
	{
	
	}
	
	void ObjectReplicator::onConnectionRemoved (ZCom_ConnID _cid, eZCom_NodeRole _remoterole)
	{
	
	}
	
	void ObjectReplicator::onDataReceived (ZCom_ConnID _cid, eZCom_NodeRole _remoterole, ZCom_BitStream &_stream, bool _store, zU32 _estimated_time_sent)
	{
	
	}
	
	void ObjectReplicator::onLocalRoleChanged (eZCom_NodeRole _oldrole, eZCom_NodeRole _newrole)
	{
	
	}
	
	void ObjectReplicator::onPacketReceived (ZCom_ConnID _cid)
	{
	
	}
	
	void ObjectReplicator::onPreSendData (ZCom_ConnID _cid, eZCom_NodeRole _remoterole, zU32 *_lastupdate)
	{
	
	}
	
	void ObjectReplicator::onRemoteRoleChanged (ZCom_ConnID _cid, eZCom_NodeRole _oldrole, eZCom_NodeRole _newrole)
	{
	
	}
	
	

	
	void ObjectReplicator::onDataAcked(ZCom_ConnID _cid, zU32 _reference_id, ZCom_BitStream *_data)
	{
	
	}
	
	void ObjectReplicator::onDataLost(ZCom_ConnID _cid, zU32 _reference_id, ZCom_BitStream *_data)
	{
	
	}
*/
}
