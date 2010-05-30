#pragma once
#include "BaseWorld.h"
#include "Camera.h"
#include "NetObject.h"

namespace Sarona
{

	class NetWorld
		: public BaseWorld
		, public IEventReceiver
	{
		scoped_ptr<Camera> m_camera;
		boost::thread		m_thread;
		ptr_vector<NetObject>			m_objects;

		void CreateV8Context();


		// Connection management
		bool ZCom_cbConnectionRequest(ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply);
		void ZCom_cbConnectionSpawned( ZCom_ConnID _id );
		void ZCom_cbConnectionClosed( ZCom_ConnID _id, eZCom_CloseReason _reason, ZCom_BitStream &_reasondata );
		void ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply );
		void ZCom_cbDataReceived( ZCom_ConnID _id, ZCom_BitStream &_data );
		bool ZCom_cbZoidRequest( ZCom_ConnID _id, zU8 _requested_level, ZCom_BitStream &_reason );
		void ZCom_cbZoidResult( ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason );
		void ZCom_cbNodeRequest_Dynamic( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
			eZCom_NodeRole _role, ZCom_NodeID _net_id );
		void ZCom_cbNodeRequest_Tag( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
			eZCom_NodeRole _role, zU32 _tag );
		bool ZCom_cbDiscoverRequest( const ZCom_Address &_addr, ZCom_BitStream &_request, 
			ZCom_BitStream &_reply );
		void ZCom_cbDiscovered( const ZCom_Address & _addr, ZCom_BitStream &_reply );

		// Node event interceptor interface
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
	
		ZCom_ConnID	m_serverConnectionId;
	public:
		NetWorld(IrrlichtDevice * dev);
		~NetWorld();

		void Connect(string host, bool local);
		void Loop();

		bool OnEvent(const SEvent & event );
	};

}