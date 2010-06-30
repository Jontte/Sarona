#pragma once
#include "StdAfx.h"

namespace Sarona
{
	class PhysObject;
	class NetObject;

	class ObjectReplicator :
		public ZCom_ReplicatorAdvanced
	{
		// one of these will be active..
		PhysObject* m_phys;
		NetObject* m_net;

		btVector3 m_position;
		btQuaternion m_rotation;
		btVector3 m_velocity;
		btVector4 m_omega;

	private:
		void send(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega);

		std::set<ZCom_ConnID> m_connections;
		void init_replicator();

	public:
		ObjectReplicator(ZCom_ReplicatorSetup * setup, PhysObject * phys);
		ObjectReplicator(ZCom_ReplicatorSetup * setup, NetObject * net);
		~ObjectReplicator(void); 

		void clearPeekData ();
		void* peekData();
		void Process (eZCom_NodeRole _localrole, zU32 _simulation_time_passed);

		void Input(const btVector3& position, const btQuaternion& rotation, const btVector3& velocity, const btVector4& omega);
		
		void onConnectionAdded (ZCom_ConnID _cid, eZCom_NodeRole _remoterole);
		void onConnectionRemoved (ZCom_ConnID _cid, eZCom_NodeRole _remoterole);
		void onDataReceived (ZCom_ConnID _cid, eZCom_NodeRole _remoterole, ZCom_BitStream &_stream, bool _store, zU32 _estimated_time_sent);
		void onLocalRoleChanged (eZCom_NodeRole _oldrole, eZCom_NodeRole _newrole);
		void onPacketReceived (ZCom_ConnID _cid);
		void onPreSendData (ZCom_ConnID _cid, eZCom_NodeRole _remoterole, zU32 *_lastupdate);
		void onRemoteRoleChanged (ZCom_ConnID _cid, eZCom_NodeRole _oldrole, eZCom_NodeRole _newrole);

		
		void onDataAcked(ZCom_ConnID _cid, zU32 _reference_id, ZCom_BitStream *_data);
		void onDataLost(ZCom_ConnID _cid, zU32 _reference_id, ZCom_BitStream *_data);
		
	};
}
