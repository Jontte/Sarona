#pragma once
#include "BaseWorld.h"
#include "Camera.h"
#include "NetObject.h"

namespace Sarona
{

	class NetWorld : public BaseWorld
	{
		scoped_ptr<Camera> m_camera;
		boost::thread		m_thread;
		ptr_vector<NetObject>			m_objects;

		void CreateV8Context();

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
	
	public:
		NetWorld(IrrlichtDevice * dev);
		~NetWorld();

		void Connect(string host, bool local);
		void Loop();
	};

}