#include "StdAfx.h"
#include "PhysWorld.h"

#include <stdexcept>

namespace Sarona
{
	PhysWorld::PhysWorld(IrrlichtDevice* device, NetworkCtx* netctx) : BaseWorld(device, netctx)
	{
		// Load script, initialize components
		CreateV8Context();
	}

	PhysWorld::~PhysWorld(void)
	{
		m_objects.clear(); // Objects must die before the phys engine dies...
	}

	void PhysWorld::CreateV8Context()
	{
		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

		// Scene
		v8::Handle<v8::ObjectTemplate> scene = v8::ObjectTemplate::New();
		scene->SetInternalFieldCount(1);
		scene->Set(v8::String::New("create"), v8::FunctionTemplate::New(PhysWorld::JSSceneCreate));
		scene->SetAccessor(v8::String::New("gravity"), PhysWorld::JSSceneGetter, PhysWorld::JSSceneSetter);

		// Static functions
		global->Set(v8::String::New("print"), v8::FunctionTemplate::New(PhysWorld::JSPrint));

		// Objects
		global->Set(v8::String::New("scene"), scene);

		// Constants
		global->Set(v8::String::New("SHAPE_PLANE"), v8::Int32::New(0));
		global->Set(v8::String::New("SHAPE_SPHERE"), v8::Int32::New(1));

		m_jscontext = v8::Context::New(NULL, global);

		v8::Context::Scope scope(m_jscontext);
		
		v8::Handle<v8::External> myself = v8::External::New(this);

		// Set 'this' pointer to 'scene'
		m_jscontext->Global()->Get(v8::String::New("scene"))->ToObject()->SetInternalField(0, myself);
		//m_jscontext->Global()->Get(v8::String::New("scene"))->ToObject()->Get(v8::String::New("create"))->ToObject()->SetInternalField(0, myself);
	}

	void PhysWorld::CreateBulletContext()
	{
		m_broadphase.reset(new btDbvtBroadphase());

		m_collisionConfiguration.reset(new btDefaultCollisionConfiguration());
        m_dispatcher.reset(new btCollisionDispatcher(get_pointer(m_collisionConfiguration)));

		m_solver.reset(new btSequentialImpulseConstraintSolver);
		m_dynamicsWorld.reset(new btDiscreteDynamicsWorld(
			get_pointer(m_dispatcher),
			get_pointer(m_broadphase),
			get_pointer(m_solver),
			get_pointer(m_collisionConfiguration)
			));

		m_dynamicsWorld->setGravity(btVector3(0,0,-9.80665f));
	}

	void PhysWorld::Start()
	{
		// Start physics
		CreateBulletContext();

		// Call 'level_start' defined in script

		v8::HandleScope handle_scope;
		v8::Context::Scope scope(m_jscontext);

		v8::TryCatch trycatch;
		v8::Handle<v8::Function> level_start = v8::Handle<v8::Function>::Cast(m_jscontext->Global()->Get(v8::String::New("level_start")));
		
		v8::Handle<v8::Value> result = level_start->Call(level_start, 0, NULL);
		if(result.IsEmpty())
		{
			v8::Handle<v8::Value> exception = trycatch.Exception();
			v8::String::AsciiValue exception_str(exception);

			std::cout << "Exception: " << *exception_str << std::endl;
		}

		//CreateCube(btVector3(0,0,10), 5)->setMass(1);
		//CreateCube(btVector3(-4.5,-4.5,16), 5)->setMass(100);
		CreatePlane(btVector3(0,0,0), btVector3(0,0,1));
	}


	PhysObject* PhysWorld::CreateCube(const btVector3& position, const btScalar& length)
	{
		scene::IMeshSceneNode* node;

		btCollisionShape* shape;

		node = m_device->getSceneManager()->addCubeSceneNode(length);
		node->setMaterialFlag(video::EMF_LIGHTING, true);
//		node->setMaterialFlag(video::EMF_, true);

		m_device->getSceneManager()->getMeshManipulator()->recalculateNormals(node->getMesh(), true, true);

		if(node->getMaterialCount() > 0)
		{
			// set texture..
			node->setMaterialTexture( 0,m_device->getVideoDriver()->getTexture("testgraphics.png"));
		}
		
		shape = new btBoxShape(btVector3(length/2, length/2, length/2));

		PhysObject* obj = new PhysObject(get_pointer(m_device), node, get_pointer(m_dynamicsWorld), shape, btTransform(btQuaternion(0,0,0,1),position));
		obj->setMass(1);
		m_objects.push_back(obj);
		return obj;
	}
	
	PhysObject* PhysWorld::CreateSphere(const btVector3& position, const btScalar& radius)
	{
		return NULL;
	}
	
	PhysObject* PhysWorld::CreatePlane(const btVector3& position, const btVector3& normal)
	{
		scene::ISceneNode* node;
		btCollisionShape* shape;

		// Create mesh
		scene::IMesh* mesh = m_device->getSceneManager()->addHillPlaneMesh("hillplane", core::dimension2d<f32>(10.0f, 10.0f), core::dimension2d<u32>(100,100))
				->getMesh(0);

		// Rotate it properly (z-axis is up)
		core::matrix4 matrix;
		matrix.setRotationDegrees (core::vector3df(90,0,0));
		m_device->getSceneManager()->getMeshManipulator()->transform(mesh, matrix);	

		m_device->getSceneManager()->getMeshManipulator()->recalculateNormals(mesh, true, true);

		// Create node
		node = m_device->getSceneManager()->addMeshSceneNode(mesh);
		shape = new btStaticPlaneShape(normal, 0);

		PhysObject* obj = new PhysObject(get_pointer(m_device), node, get_pointer(m_dynamicsWorld), shape, btTransform(btQuaternion(0,0,0,1),position));
		m_objects.push_back(obj);
		return obj;
	}

	void PhysWorld::Step(float dt)
	{
		m_dynamicsWorld->stepSimulation(dt,1);
	}


	v8::Handle<v8::Value> PhysWorld::SceneCreate(const v8::Arguments& args)
	{
		if(args.Length() != 2)
			return v8::ThrowException(v8::String::New("scene.create() must be called with exactly 2 arguments"));
		if(!args[0]->IsObject())
			return v8::ThrowException(v8::String::New("first parameter to scene.create() must be an object!"));
		if(!args[1]->IsArray())
			return v8::ThrowException(v8::String::New("second parameter to scene.create() must be an array!"));
		

		btVector3 position;


		v8::Handle<v8::Object> def = args[0]->ToObject();
		v8::Handle<v8::Object> pos = args[1]->ToObject();


		// Parse position vector

		v8::Local<v8::Array> names = pos->GetPropertyNames();
		
		int dim = names->Length();
		if(dim != 3)
			return v8::ThrowException(v8::String::New("position vector invalid"));

		position.setX((float)pos->Get(0)->ToNumber()->Value());
		position.setY((float)pos->Get(1)->ToNumber()->Value());
		position.setZ((float)pos->Get(2)->ToNumber()->Value());
		
		// Parse object
		v8::Handle<v8::Object> shapes = def->Get(v8::String::New("shapes"))->ToObject();

		names = shapes->GetPropertyNames();
		
		int shapecount = names->Length();

		for(int i = 0; i < shapecount ; i++)
		{
			v8::Handle<v8::Object> shape = shapes->Get(names->Get(i))->ToObject();
			
			float mass = (float)shape->Get(v8::String::New("mass"))->ToNumber()->Value();

			v8::Handle<v8::String> shapetype = shape->Get(v8::String::New("shape"))->ToString();

			std::string stringtype(*v8::String::Utf8Value(shapetype));

			if(stringtype == "plane")
			{
				// read normal vector
			}
			else if(stringtype == "sphere")
			{
				// read radius
			}
			else if(stringtype == "cube")
			{
				// read length

				float length = (float)shape->Get(v8::String::New("length"))->ToNumber()->Value();

				CreateCube(position, length)->setMass(mass);
			}
			else
			{
				return v8::ThrowException(v8::String::New("unknown shape"));
			}

		}
		
		return v8::Handle<v8::Value>();
	}
	v8::Handle<v8::Value> PhysWorld::SceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		return v8::Handle<v8::Value>();
	}
	void PhysWorld::SceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		return;
	}
}