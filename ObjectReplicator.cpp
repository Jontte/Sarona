#include "StdAfx.h"
#include "ObjectReplicator.h"
#include "NetObject.h"
#include "PhysObject.h"

namespace Sarona
{

	static const int FLOAT_BITS = 12;

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
		init_replicator();
	}

	ObjectReplicator::~ObjectReplicator()
	{
	}

	void ObjectReplicator::init_replicator()
	{
		m_position[0] = m_position[1] = m_position[2] = 0;

		m_setup->setMinDelay(10);

		//m_flags |= ZCOM_REPLICATOR_CALLPROCESS;
		m_flags |= ZCOM_REPLICATOR_INITIALIZED;
	}

	void ObjectReplicator::send(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega)
	{
		ZCom_BitStream stream;

		// position
		stream.addFloat(position[0], FLOAT_BITS);
		stream.addFloat(position[1], FLOAT_BITS);
		stream.addFloat(position[2], FLOAT_BITS);
		// rotation
		stream.addFloat(rotation[0], FLOAT_BITS);
		stream.addFloat(rotation[1], FLOAT_BITS);
		stream.addFloat(rotation[2], FLOAT_BITS);
		stream.addFloat(rotation[3], FLOAT_BITS);
		// velocity
		stream.addFloat(velocity[0], FLOAT_BITS);
		stream.addFloat(velocity[1], FLOAT_BITS);
		stream.addFloat(velocity[2], FLOAT_BITS);
		// omega
		stream.addFloat(omega[0], FLOAT_BITS);
		stream.addFloat(omega[1], FLOAT_BITS);
		stream.addFloat(omega[2], FLOAT_BITS);
		stream.addFloat(omega[3], FLOAT_BITS);

		for(std::set<ZCom_ConnID>::iterator iter = m_connections.begin(); iter != m_connections.end(); iter++)
		{
			zU32* lastupdate = getLastUpdateTime(*iter);
			if (!lastupdate)
				continue;

			if ((ZoidCom::getTime() - *lastupdate) < (zU32)m_setup->getMinDelay())
			{
				// don't send data if last update to this object happened too recently
				continue;
			}
			*lastupdate = ZoidCom::getTime();

			this->sendDataDirect(eZCom_Unreliable, *iter, stream.Duplicate()); 
//			this->sendDataDirect(eZCom_ReliableUnordered, *iter, stream.Duplicate()); 
		}
	}
	
	void ObjectReplicator::Input(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega)
	{
		// mark dirty if position change significant

		//btVector3 temp = position - m_position;

		//if(velocity.length2() > (0.125*0.125))
		{
			m_position = position;

			send(position, rotation, velocity, omega);
		}
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
		position[0] = _stream.getFloat(FLOAT_BITS);
		position[1] = _stream.getFloat(FLOAT_BITS);
		position[2] = _stream.getFloat(FLOAT_BITS);
		// rotation
		rotation[0] = _stream.getFloat(FLOAT_BITS);
		rotation[1] = _stream.getFloat(FLOAT_BITS);
		rotation[2] = _stream.getFloat(FLOAT_BITS);
		rotation[3] = _stream.getFloat(FLOAT_BITS);
		// velocity
		velocity[0] = _stream.getFloat(FLOAT_BITS);
		velocity[1] = _stream.getFloat(FLOAT_BITS);
		velocity[2] = _stream.getFloat(FLOAT_BITS);
		// omega
		omega[0] = _stream.getFloat(FLOAT_BITS);
		omega[1] = _stream.getFloat(FLOAT_BITS);
		omega[2] = _stream.getFloat(FLOAT_BITS);
		omega[3] = _stream.getFloat(FLOAT_BITS);

		if(m_net)
		{
			core::vector3df pos(position[0], position[1], position[2]);
			core::quaternion rot;
			
			rot.X =	rotation.x();
			rot.Y =	rotation.y();
			rot.Z =	rotation.z();
			rot.W =	rotation.w();

			rot.normalize(); // get rid of rounding errors etc.

			core::vector3df vel(velocity[0], velocity[1], velocity[2]);

			float omg[4];
			omg[0]= omega[0];
			omg[1]= omega[1];
			omg[2]= omega[2];
			omg[3]= omega[3];

			boost::posix_time::ptime when = boost::posix_time::microsec_clock::local_time();
			// subtract ping..
			when -= boost::posix_time::milliseconds(ZoidCom::getTime() - _estimated_time_sent);

			m_net->UpdateLastKnown(pos, rot, vel, omg, when);
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
