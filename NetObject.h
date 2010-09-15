#pragma once
#ifndef NETOBJECT_H_
#define NETOBJECT_H_
#include "StdAfx.h"

namespace Sarona
{
	class NetObject
		: public ZCom_NodeEventInterceptor // Receive regular events
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

		// Is this object waiting for destruction?
		bool m_deleteme;

		scene::IMeshSceneNode*		m_sceneNode;
		IrrlichtDevice*				m_device;

		// Last known position data:
		core::vector3df m_position;
		core::quaternion m_rotation;
		core::vector3df m_velocity;
		float m_omega[4];
		boost::posix_time::ptime m_when;

		// Synchronized data
		string m_mesh;
		string m_texture;
		float m_meshScale;

		void reloadNode();
	public:
		typedef ZCom_NodeID Id; // Used to implement weak refs to these object

		void RefreshPosRot();

		void UpdateLastKnown(
			const core::vector3df& position, const core::quaternion& rotation,
			const core::vector3df& velocity, const float m_omega[4],
			const boost::posix_time::ptime& when);

		// Getters
		core::vector3df getPosition();

		// Returns whether this object is waiting to be deleted
		bool IsZombie();

		NetObject(ZCom_Control* control, IrrlichtDevice* );
		~NetObject(void);
	};
}
#endif
