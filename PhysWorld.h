#pragma once
#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"
#include "TimedEventReceiver.h"

namespace Sarona
{
	// Forward decl.
	class PhysWorld;

	// Event used to call JavaScript code (used by setTimeout)
	class JSEvent : public Event
	{
	private:
		PhysWorld * m_owner;
		v8::Persistent<v8::Value> m_function;
	public:
		JSEvent(PhysWorld* owner, v8::Handle<v8::Value> val) 
			: m_owner(owner)
			, m_function(val)
		{}
		~JSEvent()
		{
			m_function.Dispose();
		}
		void operator()();
	};
	
	class PhysWorld 
		: public BaseWorld<PhysWorld, PhysObject>
		, public TimedEventReceiver
	{
		friend class JSEvent;
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
		struct Client
		{
		private:
			struct Event
			{
				string eventtype;
				string eventname;
				v8::Persistent<v8::Function> function;
				v8::Persistent<v8::Object> context;

				~Event() {function.Dispose(); context.Dispose();}
			};

			ptr_vector<Event> m_events;

		public:
			string name; // TODO: Implement
			ZCom_ConnID connection_id;
			bool level_confirmed; 
			bool disconnected;

			Client() : level_confirmed(false), connection_id(0), name("Unnamed"), disconnected(true){}

			void bind_event(const string& eventtype, const string& eventname, v8::Handle<v8::Function> func, v8::Handle<v8::Object> context);
			void call_event(const string& eventtype, const string& eventname, std::vector<v8::Handle< v8::Value > >& args = std::vector<v8::Handle< v8::Value > >());
		};

		ptr_vector<Client> m_clients;

		Client* GetClientById(const ZCom_ConnID&);
		
		void AnnounceGameStart();

		// Ground plane
		scoped_ptr<btRigidBody>	m_ground;

		// v8
		void CreateV8Context();
		void RunSceneJS();

		v8::Handle<v8::Value>			PlayerBind(const v8::Arguments& args);
		v8::Handle<v8::Value>			PlayerCameraFollow(const v8::Arguments& args);
		v8::Handle<v8::Value>			PlayerCameraSet(const v8::Arguments& args);
		
		// static JS Functions:
		static v8::Handle<v8::Value>	JSPrint(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSPlayerBind(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSPlayerCameraFollow(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSPlayerCameraSet(const v8::Arguments& args);

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

		// Call a JS function in the context of this world, return result
		v8::Handle<v8::Value> CallFunction(v8::Handle<v8::Value> func, int argc = 0, v8::Handle<v8::Value>* argv = NULL);
	
	public:

		// Create an object in the world
		PhysObject* CreateObject(const btVector3& position);
		
		PhysWorld(IrrlichtDevice * dev = NULL);
		~PhysWorld();

		void Bind(int port, bool local = false);

		void Start();
		void Wait();

		btCollisionShape* getShape(const string& byname, bool isStatic);
	};

}
