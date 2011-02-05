#pragma once
#ifndef PHYSWORLD_H_
#define PHYSWORLD_H_
#include <stdexcept>
#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"
#include "JSObject.h"
#include "Util.h"
#include "JSVector.h"
#include "PacketDef.h"
#include "TimedEventReceiver.h"
#include "JSConvert.h"
#include "Client.h"

namespace Sarona
{
	// Forward decl.
	class PhysWorld;


	class JSTimeoutEvent;
	class TimedEventReceiver;

	class PhysWorld
		: public BaseWorld<PhysWorld, PhysObject>
	{
		friend class JSTimeoutEvent;
	private:

		boost::thread		m_thread;

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

		// Bullet
		scoped_ptr<btBroadphaseInterface>				m_broadphase;
		scoped_ptr<btDefaultCollisionConfiguration>		m_collisionConfiguration;
		scoped_ptr<btCollisionDispatcher>				m_dispatcher;
		scoped_ptr<btSequentialImpulseConstraintSolver> m_solver;
		scoped_ptr<btDiscreteDynamicsWorld>				m_dynamicsWorld;
		void CreateBulletContext();

		ptr_vector<PhysObject>			m_objects;

		// Clients

		ptr_vector<Client> 				m_clients;

		Client* GetClientById(const ZCom_ConnID&);

		void AnnounceGameStart();

		// Ground plane
		scoped_ptr<btRigidBody>	m_ground;

		// v8
		void CreateV8Context();
		void RunSceneJS();

		v8::Handle<v8::Value>			SetTimeout(const v8::Arguments& args);
		v8::Handle<v8::Value>			CreateObject(const v8::Arguments& args);

		// static JS wrappers:
		static v8::Handle<v8::Value>	JSPrint(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSSetTimeout(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSCreateObject(const v8::Arguments& args);

		void Loop();

		// Used to cache&store shapes and trianglesoups for bullet
		struct ShapeData
		{
			btCollisionShape		*	shape;
			btTriangleMesh			*	mesh_interface;
			ShapeData() : shape(NULL), mesh_interface(NULL){}
			~ShapeData(){delete shape; delete mesh_interface;}
		};

		ptr_map<string, ShapeData> m_shapecache;

		// Timer interface
		scoped_ptr<TimedEventReceiver>	m_timer;

		// Remove objects scheduled for deletion
		void KillZombies();

		// Send node status updates
		void UpdateNodes();

		// Create an object in the world (not in js, tho)
		PhysObject* CreateObject(const btVector3& position, const btQuaternion& rotation);
	public:

		PhysWorld(IrrlichtDevice * dev = NULL);
		~PhysWorld();

		void Bind(int port, bool local = false);

		// Starts the background thread
		void Start();

		// Sends a shutdown signal to the thread (causing Wait() to return)
		void Shutdown();

		// Waits for the thread to finish
		void Wait();

		btCollisionShape* getShape(const string& byname, bool isStatic);
	};

}
#endif
