#pragma once
#include "StdAfx.h"
#include "BaseWorld.h"
#include "PhysObject.h"

namespace Sarona
{
	class PhysWorld : public BaseWorld
	{
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

		// Bullet
		scoped_ptr<btBroadphaseInterface>				m_broadphase;
		scoped_ptr<btDefaultCollisionConfiguration>		m_collisionConfiguration;
		scoped_ptr<btCollisionDispatcher>				m_dispatcher;
		scoped_ptr<btSequentialImpulseConstraintSolver> m_solver;
		scoped_ptr<btDiscreteDynamicsWorld>				m_dynamicsWorld;
		void CreateBulletContext();
		ptr_vector<PhysObject>			m_objects;


		// v8
		void CreateV8Context();

		v8::Handle<v8::Value>			SceneCreate(const v8::Arguments& args);
		v8::Handle<v8::Value>			SceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
		void							SceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
		

		// static JS Functions:
		static v8::Handle<v8::Value>	JSPrint(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSSceneCreate(const v8::Arguments& args);
		static v8::Handle<v8::Value>	JSSceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
		static void						JSSceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

		void Loop();
	public:


		PhysObject* CreateCube(const btVector3& position, const btScalar& length);
		PhysObject* CreateSphere(const btVector3& position, const btScalar& radius);
		PhysObject* CreatePlane(const btVector3& position, const btVector3& normal);
		
		PhysWorld(IrrlichtDevice * dev = NULL);
		~PhysWorld();

		void Bind(int port, bool local = false);

		void Start();
		void Wait();
	};

}
