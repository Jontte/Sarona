#pragma once
#include "StdAfx.h"

namespace Sarona
{
	class NetObject 
		: public ZCom_NodeEventInterceptor // Receive regular events
		, public ZCom_ReplicatorBasic // Act as a simple replicator ourselves
	{
		// ZCom stuff
		scoped_ptr<ZCom_Node>							m_zcomNode;
		

		// Regular events
		bool recUserEvent(ZCom_Node *_node, ZCom_ConnID _from, 
						eZCom_NodeRole _remoterole, ZCom_BitStream &_data, 
						zU32 _estimated_time_sent);
		                          
		bool recInit(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole);
		bool recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from, 
						  eZCom_NodeRole _remoterole);
		                              
		bool recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole);
		                        
		bool recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
						   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid, 
						   ZCom_BitStream &_request);
		                             
		bool recFileData(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) ;
		                     
		bool recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
						  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) ;
		                           
		bool recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
						   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid);

		// Replicator methods:
		bool checkState ();
		void packData (ZCom_BitStream *_stream);
		void unpackData (ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent);
		void* peekData();
		void clearPeekData();
		void Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed);

		// Is this object waiting for destruction?
		bool m_deleteme;

		scene::IMeshSceneNode*	m_sceneNode;
		IrrlichtDevice* m_device;

		// Last known position data:
		core::vector3df m_position;
		core::quaternion m_rotation;
		core::vector3df m_velocity;
		float m_omega[4];
		boost::posix_time::ptime m_when;

		// Synchronized data
		char m_mesh[65];
		char m_texture[65];

		void reloadMesh(string filename);
		void reloadTexture(string filename);

	public:
		void RefreshPosRot(); 

		void UpdateLastKnown(
			const core::vector3df& position, const core::quaternion& rotation,
			const core::vector3df& velocity, const float m_omega[4],
			const boost::posix_time::ptime& when);

		NetObject(ZCom_Control* control, IrrlichtDevice* );
		~NetObject(void);
	};
}
