#include "StdAfx.h"
#include "PhysWorld.h"
#include "JSObject.h"

#include <stdexcept>

namespace Sarona
{
	PhysWorld::PhysWorld(IrrlichtDevice * dev)
		: BaseWorld(dev)
	{
		ZCom_setUpstreamLimit(100000, 100000);
		// Load script, initialize components
		CreateV8Context();
	}

	PhysWorld::~PhysWorld(void)
	{
		// Remove ground plane
		if(m_ground)
			m_dynamicsWorld->removeRigidBody(get_pointer(m_ground));

		m_ground.reset();

		m_objects.clear(); // Objects must die before the phys engine dies...
	}

	void PhysWorld::Bind(int port, bool local)
	{
		// Bind
		bool result = this->ZCom_initSockets(!local, port, port);
		if (!result)
		{
			throw std::runtime_error("Unable to create ZCom socket!");
		}
	}

	void PhysWorld::Start()
	{
		m_thread = boost::thread(&PhysWorld::Loop, this);
	}

	void PhysWorld::Wait()
	{
		m_thread.join();
	}

	void PhysWorld::CreateV8Context()
	{
		v8::Locker locker;

		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
		global->SetInternalFieldCount(2);

		// Scene
//		v8::Handle<v8::ObjectTemplate> scene = v8::ObjectTemplate::New();
//		scene->SetInternalFieldCount(1);

		// Static functions
//		global->Set(v8::String::New("print"), v8::FunctionTemplate::New(PhysWorld::JSPrint));

		// Objects
//		global->Set(v8::String::New("scene"), scene);

		CProxyV8::ProxyClass<JSObject>* pJSObject = CProxyV8::ProxyClass<JSObject>::instance()->init("Object");

		pJSObject->Expose(&JSObject::push, "push");
		global->Set(v8::String::New("Object"), pJSObject->GetFunctionTemplate());



		// Constants
//		global->Set(v8::String::New("SHAPE_PLANE"), v8::Int32::New(0));
//		global->Set(v8::String::New("SHAPE_SPHERE"), v8::Int32::New(1));

		m_jscontext = v8::Context::New(NULL, global);

		v8::Context::Scope scope(m_jscontext);
		
		v8::Handle<v8::External> myself = v8::External::New(this);
		v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())->SetInternalField(0, myself);
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

	void PhysWorld::Loop()
	{
		// Start physics
		CreateBulletContext();

		// Call 'level_start' defined in script
		v8::Locker locker;

		v8::HandleScope handle_scope;
		v8::Context::Scope scope(m_jscontext);

		v8::TryCatch trycatch;
		v8::Handle<v8::Function> level_start = v8::Handle<v8::Function>::Cast(m_jscontext->Global()->Get(v8::String::New("level_start")));
		
		v8::Handle<v8::Value> result = level_start->Call(level_start, 0, NULL);
		if(result.IsEmpty())
		{
			v8::Handle<v8::Value> exception = trycatch.Exception();
			v8::Handle<v8::Message> message = trycatch.Message();

			v8::String::AsciiValue exception_str(exception);

			std::cout << "Exception: " << *exception_str << std::endl;
			std::cout << "	: " << *v8::String::AsciiValue(message->GetSourceLine()) << std::endl;
		}

		//CreateCube(btVector3(0,0,10), 5)->setMass(1);
		//CreateCube(btVector3(-4.5,-4.5,16), 5)->setMass(100);
//		CreatePlane(btVector3(0,0,0), btVector3(0,0,1));

		// Create ground
		btCollisionShape* shape = new btStaticPlaneShape(btVector3(0,0,1), 0);
		btRigidBody::btRigidBodyConstructionInfo CI(0,NULL,shape,btVector3(0,0,0));
		m_ground.reset(new btRigidBody(CI));
		m_dynamicsWorld->addRigidBody(get_pointer(m_ground));

		unsigned long long frame = 0;

		// Set our delta time and time stamp

		int targetFPS = 50;

		//while(m_device->run())
		boost::posix_time::time_duration loop_avg;
		while(true)
		{
			// do stuff on the server...

			boost::posix_time::ptime beginframe = boost::posix_time::microsec_clock::local_time();

			// 1. Send/Receive messages from network
			// 2. Simulate physics
			m_dynamicsWorld->stepSimulation(btScalar(1.0/targetFPS), 1);

			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();
			//this->ZCom_processReplicators(zU32(1000.0/targetFPS));

			
			boost::posix_time::ptime endframe = boost::posix_time::microsec_clock::local_time();

			ZCom_ConnStats stats = ZCom_getConnectionStats(1);

			if(frame % (targetFPS) == 0)
			{	
				std::cout << m_objects.size() << " objects. Avg loop: " << loop_avg.total_microseconds() << " micros" << std::endl;
			}

			// Sleep the rest of the frame...

			long long int micros = 1000000/targetFPS;
			boost::posix_time::time_duration this_frame = boost::posix_time::time_period(beginframe, endframe).length();
			micros -= this_frame.total_microseconds();

			loop_avg = loop_avg /2 + this_frame/2;

			if(micros > 0)
			{
				boost::this_thread::sleep(boost::posix_time::microseconds(micros));
			}
			else
			{
				//std::cout << "Time dilatation..." << std::endl;
			}

			frame++;
		}
	}

	PhysObject* PhysWorld::CreateObject(const btVector3& position)
	{
		PhysObject* obj = new PhysObject(this, get_pointer(m_dynamicsWorld), btTransform(btQuaternion(0,0,0,1), position));
		m_objects.push_back(obj);
		return obj;
	}


	btCollisionShape* PhysWorld::getShape(const string& shape)
	{
		// is the shape cached?
		if(m_shapecache.find(shape) != m_shapecache.end())
		{
			ShapeData & data = m_shapecache[shape];
			return data.shape;
		}
		// Create index
		ShapeData & data = m_shapecache[shape];

		// Predefined shape
		if(shape == "cube")
		{
			float length = 1;
			data.shape = new btBoxShape(btVector3(length/2, length/2, length/2));
			data.mesh_interface = NULL;
			return data.shape;
		}

		// Custom shape
		// load body from file using irrlicht

		scene::IAnimatedMesh * irrmesh = m_device->getSceneManager()->getMesh(shape.c_str());
		
		data.mesh_interface = new btTriangleMesh();

		const btScalar scale = 2.5;

		for(unsigned i = 0; i < irrmesh->getMeshBufferCount(); i++)
		{
			irr::scene::IMeshBuffer * meshbuffer = irrmesh->getMeshBuffer(i);

			if(meshbuffer->getVertexType() == video::EVT_STANDARD)
			{
				irr::video::S3DVertex * vertices = reinterpret_cast<video::S3DVertex*>(meshbuffer->getVertices());
				
				u16* indices = meshbuffer->getIndices();
				int vertexcount = meshbuffer->getVertexCount();

				for(unsigned a = 0 ; a < meshbuffer->getIndexCount(); a += 3)
				{
					btVector3 triangle[3];

					for (int k=0;k<3;k++)
					{
						//three verts per triangle
						int index = indices[a+k];
						if (index > vertexcount) continue;
						//convert to btVector3
						core::vector3df& pos = vertices[index].Pos;
						triangle[k] = btVector3(pos.X, pos.Y, pos.Z) * scale;
					}
					data.mesh_interface->addTriangle(triangle[0], triangle[1], triangle[2]);
				}
			}
			else if(meshbuffer->getVertexType() == irr::video::EVT_2TCOORDS)
			{
				// Same but for S3DVertex2TCoords data
				irr::video::S3DVertex * vertices = reinterpret_cast<video::S3DVertex*>(meshbuffer->getVertices());

				u16* indices = meshbuffer->getIndices();
				int vertexcount = meshbuffer->getVertexCount();

				for(unsigned a = 0 ; a < meshbuffer -> getIndexCount(); a += 3)
				{
					btVector3 triangle[3];

					for (int k=0;k<3;k++)
					{
						s32 index = indices[a+k];
						if (index > vertexcount) continue;
						core::vector3df& pos = vertices[index].Pos * scale;
						triangle[k] = btVector3(pos.X, pos.Y, pos.Z);
					}
					data.mesh_interface->addTriangle(triangle[0], triangle[1], triangle[2]);
				}
			}
		}
		data.shape = new btConvexTriangleMeshShape(data.mesh_interface);
		return data.shape;
	}

	bool PhysWorld::ZCom_cbConnectionRequest(ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply)
	{
		return true; // Allow incoming connection
	}
	void PhysWorld::ZCom_cbConnectionSpawned( ZCom_ConnID _id )
	{
		ZCom_requestDownstreamLimit(_id, (zU16)1000, (zU16)100000 );
		//ZCom_simulateLoss(_id, 0.5);
	}
	void PhysWorld::ZCom_cbConnectionClosed( ZCom_ConnID _id, eZCom_CloseReason _reason, ZCom_BitStream &_reasondata )
	{
	
	}
	void PhysWorld::ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply )
	{
	
	}
	void PhysWorld::ZCom_cbDataReceived( ZCom_ConnID _id, ZCom_BitStream &_data )
	{
	
	}
	bool PhysWorld::ZCom_cbZoidRequest( ZCom_ConnID _id, zU8 _requested_level, ZCom_BitStream &_reason )
	{
		if (_requested_level == 1)
		  return true;
		else
		  return false;
	}
	void PhysWorld::ZCom_cbZoidResult( ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason )
	{
	
	}
	void PhysWorld::ZCom_cbNodeRequest_Dynamic( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
		eZCom_NodeRole _role, ZCom_NodeID _net_id )
	{
	
	}
	void PhysWorld::ZCom_cbNodeRequest_Tag( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
		eZCom_NodeRole _role, zU32 _tag )
	{
	
	}
	bool PhysWorld::ZCom_cbDiscoverRequest( const ZCom_Address &_addr, ZCom_BitStream &_request, 
		ZCom_BitStream &_reply )
	{
		return false;
	}
	void PhysWorld::ZCom_cbDiscovered( const ZCom_Address & _addr, ZCom_BitStream &_reply )
	{
	
	}
}