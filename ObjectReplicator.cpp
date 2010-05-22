#include "StdAfx.h"
#include "ObjectReplicator.h"
#include "NetObject.h"
#include "PhysObject.h"

namespace Sarona
{

	ObjectReplicator::ObjectReplicator(ZCom_ReplicatorSetup* setup, PhysObject* phys) 
		: ZCom_ReplicatorAdvanced(setup)
		, m_phys(phys)
		, m_net(NULL)
	{
		init_replicator();
	}
	ObjectReplicator::ObjectReplicator(ZCom_ReplicatorSetup* setup, NetObject* net) 
		: ZCom_ReplicatorAdvanced(setup)
		, m_net(net)
		, m_phys(NULL)
	{
		m_position[0] = m_position[1] = m_position[2] = 10000;
		m_setup->setMinDelay(10);
		init_replicator();
	}

	void ObjectReplicator::init_replicator()
	{
		//m_flags |= ZCOM_REPLICATOR_CALLPROCESS;
		m_flags |= ZCOM_REPLICATOR_INITIALIZED;
	}

	void ObjectReplicator::send(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega)
	{
		std::set<ZCom_ConnID>::iterator iter = m_connections.begin();

		while(iter != m_connections.end())
		{
			zU32* lastupdate = getLastUpdateTime(*iter);
			if (!lastupdate)
				return;

			if ((ZoidCom::getTime() - *lastupdate) < (zU32)m_setup->getMinDelay())
			{
				// don't send data if last update to this happened too recent
				return;
			}
			*lastupdate = ZoidCom::getTime();
			

			ZCom_BitStream* stream = new ZCom_BitStream();

			// position
			stream->addFloat(position[0], 10);
			stream->addFloat(position[1], 10);
			stream->addFloat(position[2], 10);
			// rotation
			stream->addFloat(rotation[0], 10);
			stream->addFloat(rotation[1], 10);
			stream->addFloat(rotation[2], 10);
			stream->addFloat(rotation[3], 10);
			// velocity
			stream->addFloat(velocity[0], 10);
			stream->addFloat(velocity[1], 10);
			stream->addFloat(velocity[2], 10);
			// omega
			stream->addFloat(omega[0], 10);
			stream->addFloat(omega[1], 10);
			stream->addFloat(omega[2], 10);
			stream->addFloat(omega[3], 10);

			this->sendDataDirect(eZCom_Unreliable, *iter, stream); 
//			this->sendDataDirect(eZCom_ReliableUnordered, *iter, stream); 

			iter++;
		}
	}
	
	void ObjectReplicator::Input(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega)
	{
		// mark dirty if position change significant

		btVector3 temp = position - m_position;

		if(temp.length2() > (0.125*0.125))
		{
			m_position = position;

			send(position, rotation, velocity, omega);
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
		//send();
	}
	
	void ObjectReplicator::onConnectionAdded (ZCom_ConnID _cid, eZCom_NodeRole _remoterole)
	{
		m_connections.insert(_cid);

//		send();
	}
	
	void ObjectReplicator::onConnectionRemoved (ZCom_ConnID _cid, eZCom_NodeRole _remoterole)
	{
		m_connections.erase(_cid);
	}
	
	void ObjectReplicator::onDataReceived (ZCom_ConnID _cid, eZCom_NodeRole _remoterole, ZCom_BitStream &_stream, bool _store, zU32 _estimated_time_sent)
	{
		btVector3 position;
		btQuaternion rotation;
		btVector3 velocity;
		btVector4 omega;

		// position
		position[0] = _stream.getFloat(10);
		position[1] = _stream.getFloat(10);
		position[2] = _stream.getFloat(10);
		// rotation
		rotation[0] = _stream.getFloat(10);
		rotation[1] = _stream.getFloat(10);
		rotation[2] = _stream.getFloat(10);
		rotation[3] = _stream.getFloat(10);
		// velocity
		velocity[0] = _stream.getFloat(10);
		velocity[1] = _stream.getFloat(10);
		velocity[2] = _stream.getFloat(10);
		// omega
		omega[0] = _stream.getFloat(10);
		omega[1] = _stream.getFloat(10);
		omega[2] = _stream.getFloat(10);
		omega[3] = _stream.getFloat(10);

		if(m_net)
		{
			core::vector3df pos(position[0], position[1], position[2]);
			core::quaternion rot;
			
			rot.X =	rotation.x();
			rot.Y =	rotation.y();
			rot.Z =	rotation.z();
			rot.W =	rotation.w();

			rot.normalize(); // get rid of rounding errors etc.

			m_net->SetTransform(pos, rot);
		}
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

}
