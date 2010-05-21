#pragma once
#include "StdAfx.h"

namespace Sarona
{
	class NetObject : public ZCom_NodeEventInterceptor
	{
		// ZCom stuff
		scoped_ptr<ZCom_Node>							m_zcomNode;
		
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

		scene::ISceneNode*	m_sceneNode;
		IrrlichtDevice* m_device;
	public:
		void SetPosition(const core::vector3df& position);

		NetObject(ZCom_Control* control, IrrlichtDevice* );
		~NetObject(void);
	};
}
