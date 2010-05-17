#pragma once
#include "StdAfx.h"
#include "Object.h"

namespace Sarona
{
class World
{
private:


	// Level config
	std::string m_name;
	std::string m_description;
	std::string m_author;

	// Irrlicht 
	intrusive_ptr<IrrlichtDevice>	m_device;

	// V8
	v8::Persistent<v8::Context>		m_jscontext;
	void CreateV8Context();

	// Bullet
	scoped_ptr<btBroadphaseInterface>				m_broadphase;
	scoped_ptr<btDefaultCollisionConfiguration>		m_collisionConfiguration;
	scoped_ptr<btCollisionDispatcher>				m_dispatcher;
	scoped_ptr<btSequentialImpulseConstraintSolver> m_solver;
	scoped_ptr<btDiscreteDynamicsWorld>				m_dynamicsWorld;
	void CreateBulletContext();

	// General

	ptr_vector<Object>	m_objects;


	v8::Handle<v8::Value>			SceneCreate(const v8::Arguments& args);
	v8::Handle<v8::Value>			SceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	void							SceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	
private:
	// static JS Functions:
	static v8::Handle<v8::Value>	JSPrint(const v8::Arguments& args);
	static v8::Handle<v8::Value>	JSSceneCreate(const v8::Arguments& args);
	static v8::Handle<v8::Value>	JSSceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void						JSSceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

public:


	Object* CreateCube(const btVector3& position, const btScalar& length);
	Object* CreateSphere(const btVector3& position, const btScalar& radius);
	Object* CreatePlane(const btVector3& position, const btVector3& normal);
	
	World(intrusive_ptr<IrrlichtDevice> device);
	~World();
	
	void Start();
	void Step(float dt);
};

}
