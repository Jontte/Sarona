#pragma once
#include "StdAfx.h"
#include "BaseWorld.h"
#include "BaseObject.h"
#include "PhysObject.h"

namespace Sarona
{
	class PhysWorld : public BaseWorld
	{
	private:

		// Bullet
		scoped_ptr<btBroadphaseInterface>				m_broadphase;
		scoped_ptr<btDefaultCollisionConfiguration>		m_collisionConfiguration;
		scoped_ptr<btCollisionDispatcher>				m_dispatcher;
		scoped_ptr<btSequentialImpulseConstraintSolver> m_solver;
		scoped_ptr<btDiscreteDynamicsWorld>				m_dynamicsWorld;
		void CreateBulletContext();

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

	public:


		PhysObject* CreateCube(const btVector3& position, const btScalar& length);
		PhysObject* CreateSphere(const btVector3& position, const btScalar& radius);
		PhysObject* CreatePlane(const btVector3& position, const btVector3& normal);
		
		PhysWorld(IrrlichtDevice* device, NetworkCtx* netctx = NULL);
		~PhysWorld();
		
		void Start();
		void Step(float dt);
	};

}
