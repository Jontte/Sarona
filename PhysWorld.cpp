#include "StdAfx.h"
#include "PhysWorld.h"
#include "JSConvert.h"
#include "JSVector.h"
#include "JSRotation.h"

namespace Sarona
{

	// Event used to call JavaScript code (used by setTimeout)
	class JSTimeoutEvent : public TimedEventReceiver::Event
	{
	private:
		PhysWorld * m_owner;
		v8::Persistent<v8::Value> m_function;
	public:
		JSTimeoutEvent(PhysWorld* owner, v8::Handle<v8::Value> val)
			: m_owner(owner)
		{
			m_function = v8::Persistent<v8::Value>::New(val);
		}
		~JSTimeoutEvent()
		{
			m_function.Dispose();
		}
		void operator()()
		{
			// Time to do our thing

			m_owner->CallFunction(m_function);
		}
	};


	PhysWorld::PhysWorld(IrrlichtDevice * dev)
		: BaseWorld<PhysWorld, PhysObject>(dev)
	{
		m_timer.reset(new TimedEventReceiver);
		ZCom_setUpstreamLimit(30000, 30000);

		this->beginReplicationSetup(0);
		this->endReplicationSetup();
		this->setEventInterceptor(this);
		this->registerNodeUnique(TypeRegistry::m_commId, eZCom_RoleAuthority, this);
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

	void PhysWorld::Shutdown()
	{
		m_thread.interrupt();
	}

	void PhysWorld::Wait()
	{
		m_thread.join();
	}

	void PhysWorld::KillZombies()
	{
		// Remove objects scheduled for deletion
		for(int i = 0; i < (int)m_objects.size(); i++)
		{
			PhysObject& obj = m_objects[i];
			if(obj.isZombie())
			{
				m_objects.erase(m_objects.begin() + i);
				i--;
			}
		}
	}

	void PhysWorld::UpdateNodes()
	{
		// Send node status updates
		for(int i = 0; i < (int)m_objects.size(); i++)
		{
			PhysObject& obj = m_objects[i];
			obj.sendUpdate();
		}
	}

	void PhysWorld::CreateV8Context()
	{
		// Get rid of any existing contexts..
		m_jscontext.Dispose();

		v8::Locker locker;

		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
		global->SetInternalFieldCount(2);

		v8::Handle<v8::ObjectTemplate> scene = v8::ObjectTemplate::New();
		scene->SetInternalFieldCount(1);

		// Static functions
		global->Set(v8::String::New("print"),			v8::FunctionTemplate::New(PhysWorld::JSPrint));
		global->Set(v8::String::New("setTimeout"),		v8::FunctionTemplate::New(PhysWorld::JSSetTimeout));
		global->Set(v8::String::New("createObject"),	v8::FunctionTemplate::New(PhysWorld::JSCreateObject));

		// Scene Object
		global->Set(v8::String::New("scene"), scene);

		// Init context
		m_jscontext = v8::Context::New(NULL, global);
		v8::Context::Scope scope(m_jscontext);
		v8::Handle<v8::External> myself = v8::External::New(this);
		v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())->SetInternalField(0, myself);

		v8::Handle<v8::Object> global_obj = v8::Context::GetCurrent()->Global();

		JSObject::SetupClass(global_obj);
		JSVector::SetupClass(global_obj);
		JSRotation::SetupClass(global_obj);
		Client::JSHandle::SetupClass(global_obj);
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
	Client* PhysWorld::GetClientById(const ZCom_ConnID& id)
	{
		ptr_vector<Client>::iterator iter = m_clients.begin();
		while(iter != m_clients.end())
		{
			if(iter->m_connection_id == id)
				return &*iter;
			iter++;
		}
		return NULL;
	}

	void PhysWorld::AnnounceGameStart()
	{
		m_gamerunning = true;

		// Send gamerunning event to all players

		ZCom_BitStream stream;

		Protocol::GameStartNotify notify;

		notify.write(stream);

		ptr_vector<Client>::iterator iter = m_clients.begin();
		while(iter != m_clients.end())
		{
			this->sendEventDirect(eZCom_ReliableOrdered, stream.Duplicate(), iter->m_connection_id);
			iter++;
		}
	}

	void PhysWorld::RunSceneJS()
	{
		v8::Locker locker;

		v8::HandleScope handle_scope;
		v8::Context::Scope scope(m_jscontext);

		// Create scene.players object now that we know how many players there are

		v8::Handle<v8::Value> scenevariant = v8::Context::GetCurrent()->Global()->Get(v8::String::New("scene"));

		if(!scenevariant->IsObject())
			return; // shouldn't happen unless someone did something nasty in the code

		v8::Handle<v8::Object> obj = scenevariant->ToObject();
		v8::Handle<v8::Array> players = v8::Array::New();

		for( unsigned i = 0 ; i < m_clients.size(); i++)
		{
			// Instantiate new instance of Client::JSHandle via JS:
			typedef v8::juice::cw::ClassWrap<Client::JSHandle> CW;
			v8::Handle<v8::Object> jobj = CW::Instance().NewInstance(0, NULL);

			// Get ref to the native type & implant the weak ref to PhysWorld-owned Client class
			Client::JSHandle *handle = CW::ToNative::Value(jobj);
			handle->initialize(&m_clients[i]);

			players->Set(i, jobj);
		}
		obj->Set(v8::String::New("players"), players);

		// Call 'level_start' defined in script

		v8::Handle<v8::Value> result = CallFunction(v8::String::New("level_start();"));
		if(result.IsEmpty())
		{
			// Call failed...
			std::cout << "Call failed!" << std::endl;
		}
	}

	void PhysWorld::Loop()
	{
		boost::this_thread::disable_interruption disable;
		// Wait until enough players have joined...

		while(true)
		{
			if(boost::this_thread::interruption_requested())
				return;

			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();

			boost::this_thread::sleep(boost::posix_time::milliseconds(50));

			if(!m_gamerunning && m_clients.size() > 0)
			{
				// Wait for enough confirmations to start the game
				unsigned confirmations = 0;
				ptr_vector<Client>::iterator iter = m_clients.begin();
				while(iter != m_clients.end())
				{
					if(iter->m_level_confirmed)
						confirmations++;
					iter++;
				}

				if(confirmations == m_clients.size())
				{
					// We're ready to go! Game begin!
					AnnounceGameStart();

					// Process events for 1 second and start self.
					boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
					while(true)
					{
						this->ZCom_processInput();
						this->ZCom_processOutput();
						boost::this_thread::sleep(boost::posix_time::milliseconds(50));

						boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
						boost::posix_time::time_duration timewaited = boost::posix_time::time_period(begin, now).length();

						if(timewaited > boost::posix_time::seconds(1))
							break;
					}

					break;
				}
			}
		}


		std::cout << "Game starting!" << std::endl;

		// Start physics
		CreateBulletContext();
		// Load level, initialize components
		CreateV8Context();
		// Execute scripts
		LoadLevel(true); // also load scripts
		// Create Player objects in Javascript and run level_start()
		RunSceneJS();

		//CreateCube(btVector3(0,0,10), 5)->setMass(1);
		//CreateCube(btVector3(-4.5,-4.5,16), 5)->setMass(100);
//		CreatePlane(btVector3(0,0,0), btVector3(0,0,1));

		// Create ground
		btCollisionShape* shape = new btStaticPlaneShape(btVector3(0,0,1), 0);
		btRigidBody::btRigidBodyConstructionInfo CI(0,NULL,shape,btVector3(0,0,0));
		m_ground.reset(new btRigidBody(CI));
		m_dynamicsWorld->addRigidBody(get_pointer(m_ground));

		unsigned long long frame = 0;
		unsigned long long framecounter = 0;
		// Set our delta time and time stamp

		const int targetFPS = 50;
		double frametime = 0;
		double totalframetime = 1.0/targetFPS;
		SimpleTimer timer;
		while(true)
		{
			if(boost::this_thread::interruption_requested())
				break;

			// Simulate physics
//			m_dynamicsWorld->stepSimulation(btScalar(1.0/targetFPS), 5);
			m_dynamicsWorld->stepSimulation(totalframetime, 20);

			// Remove dead objects from simulation
			KillZombies();

			// Send node status updates (mesh, texture, scale, etc. slow-changing params)
			UpdateNodes();

			// Step network code
 			this->ZCom_processInput();
			this->ZCom_processOutput();
			//this->ZCom_processReplicators(zU32(1000.0/targetFPS));

			// Advance the timer, calling timed callbacks
			m_timer->TimeStep(totalframetime);

			framecounter++;
			frame++;
			double elapsed = timer.elapsed();
			if(elapsed >= 1.0)
			{
				timer.restart();
				double lastsleep = 1.0/targetFPS-frametime;
				frametime = elapsed / framecounter;
				totalframetime = frametime;
				frametime -= lastsleep;
				std::cout << m_objects.size() << " objects. 		Server FPS: " << framecounter << " frametime " << frametime << ", " << totalframetime << std::endl;
				framecounter = 0;
			}

			// Limit fps by sleeping?
			double tosleep = 1.0/targetFPS-frametime;
			if(tosleep > 0)
			{
				boost::this_thread::sleep(boost::posix_time::microseconds(1000000.0*tosleep));
			}
			else
			{
				//std::cout << "Time dilatation..." << std::endl;
			}
		}
	}

	PhysObject* PhysWorld::CreateObject(const btVector3& position, const btQuaternion& rotation)
	{
		// Creates bare object in the dynamics simulation context
		PhysObject* obj = new PhysObject(this, get_pointer(m_dynamicsWorld), btTransform(rotation, position));
		m_objects.push_back(obj);
		return obj;
	}
	v8::Handle<v8::Value> PhysWorld::CreateObject(const v8::Arguments& arg)
	{
		using namespace extractor;

		if(arg.Length() != 1 || !arg[0]->IsObject())
			return v8::Undefined();

		v8::Handle<v8::Object> obj = arg[0]->ToObject();

		// Creates object in JS context
		std::string mesh;
		std::string body;
		std::string texture;
		double mass(0), bodyScale(1), meshScale(1);
		btVector3 position(0,0,0);
		btQuaternion rotation(btVector3(0,1,0), 0.0);

		std::map<std::string, v8::Handle<v8::Value> > params;

		try
		{
			emap(obj)
				("body",		body, 		0) // default body = cube
				("mesh",		mesh, 		0) // default mesh = cube
				("position", 	position,	REQUIRED)
				("rotation", 	rotation,	0)
				("texture", 	texture,	0)
				("mass",		mass,		0)
				("bodyScale",	bodyScale,	0)
				("meshScale",	meshScale,	0);
		}
		catch(...)
		{
			return v8::Undefined();
		}
//		std::cout << "tex " << texture << " mesh " << mesh << std::endl;
		// Success! Create the object
		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));
		PhysWorld* world = static_cast<PhysWorld*>(external->Value());

		PhysObject * object = world->CreateObject(position, rotation);

		typedef v8::juice::cw::ClassWrap<JSObject> CW;
		v8::Handle<v8::Object> jobj = CW::Instance().NewInstance(0, NULL);

		// Get ref to the native type & implant the weak ref to PhysWorld-owned Client class
		JSObject *handle = CW::ToNative::Value(jobj);
		handle -> initialize(object->getNetworkId());

		object -> setBody(body);
		object -> setMesh(mesh);
		object -> setMass(mass);
		object -> setTexture(texture);
		object -> setMeshScale(meshScale);
		object -> setBodyScale(bodyScale);

		return jobj;
	}


	v8::Handle<v8::Value> PhysWorld::SetTimeout(const v8::Arguments& args)
	{
		// First param: timeout in milliseconds
		// Second param: function to be called

		if(args.Length() != 2)return v8::Undefined();

		double milliseconds = v8::juice::convert::CastFromJS<double>(args[0]);
		v8::Handle<v8::Value> function = args[1];

		shared_ptr<JSTimeoutEvent> e = make_shared<JSTimeoutEvent>(this, function);

		m_timer->Schedule(milliseconds / 1000.0, e);

		return v8::Undefined();
	}

	btCollisionShape* PhysWorld::getShape(const string& shape, bool isStatic)
	{
		// is the shape cached?
		string cachename = shape;
		cachename += isStatic ? "-static" : "-dynamic";
		if(m_shapecache.find(cachename) != m_shapecache.end())
		{
			ShapeData & data = m_shapecache[cachename];
			return data.shape;
		}
		// Create index
		ShapeData & data = m_shapecache[cachename];
		data.mesh_interface = NULL;

		// load body from file using irrlicht unles its cub
		scene::IAnimatedMesh * irrmesh = NULL;
		if(shape == "cube" || !(irrmesh = m_device->getSceneManager()->getMesh(shape.c_str())))
		{
			float length = 1;
			data.shape = new btBoxShape(btVector3(length/2, length/2, length/2));
			return data.shape;
		}

		data.mesh_interface = new btTriangleMesh();

		// Deal some rotation to the mesh since our coordinate system has Z axis up
		/*core::matrix4 rotmatrix;
		rotmatrix.setRotationDegrees(core::vector3df(90,0,0));
		for(unsigned i = 0; i < irrmesh->getMeshBufferCount(); i++)
		{
			irr::scene::IMeshBuffer * meshbuffer = irrmesh->getMeshBuffer(i);
			m_device->getSceneManager()->getMeshManipulator()->transform(meshbuffer, rotmatrix);
		}*/


		const btScalar scale = 1.0;

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
						core::vector3df pos = vertices[index].Pos * scale;
						triangle[k] = btVector3(pos.X, pos.Y, pos.Z);
					}
					data.mesh_interface->addTriangle(triangle[0], triangle[1], triangle[2]);
				}
			}
		}
		if(isStatic)
		{
			// Static shapes can be concave
			data.shape = new btBvhTriangleMeshShape(data.mesh_interface, true, true);
		}
		else
		{
			// At the moment dynamic shapes must be convex
			// that is until we implement convex decomposition here
			// TODO: convex decomposition..
			data.shape = new btConvexTriangleMeshShape(data.mesh_interface);
		}
		return data.shape;
	}

	bool PhysWorld::ZCom_cbConnectionRequest(ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply)
	{
		// Allow incoming connection when game is not running (we're in lobby)
		return !m_gamerunning;
	}
	void PhysWorld::ZCom_cbConnectionSpawned( ZCom_ConnID _id )
	{
		ZCom_requestDownstreamLimit(_id, (zU16)30000, (zU16)30000 );
		//ZCom_simulateLoss(_id, 0.5);
	}
	void PhysWorld::ZCom_cbConnectionClosed( ZCom_ConnID _id, eZCom_CloseReason _reason, ZCom_BitStream &_reasondata )
	{
		// Mark client disconnected
		Client *c = GetClientById(_id);
		if(c)
		{
			c->m_disconnected = true;
		}
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
		{
			// Add new client
			Client * c = new Client(this);
			c->m_connection_id = _id;
			c->m_disconnected = false ; // They're active
			m_clients.push_back(c);
			return true;
		}
		else
		  return false;
	}
	void PhysWorld::ZCom_cbZoidResult( ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason )
	{
		// Send LevelSelect to new clients
		ZCom_BitStream* stream = new ZCom_BitStream;
			Protocol::LevelSelect select;
			select.name = m_levelname;
			select.write(*stream);
		this->sendEventDirect(eZCom_ReliableOrdered, stream, _id);

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

	// Node events

	bool PhysWorld::recUserEvent(ZCom_Node *_node, ZCom_ConnID _from,
					eZCom_NodeRole _remoterole, ZCom_BitStream &_data,
					zU32 _estimated_time_sent)
	{
		// Check event type:

		unsigned int Id = _data.getInt(16);

		if(Id == Protocol::KeyPressEvent::Id)
		{
			Protocol::KeyPressEvent event;
			event.read(_data);

			Client * c = GetClientById(_from);
			if(c)
			{
				// Calling an event requires a scope :p

				if(m_jscontext.IsEmpty())
					return false;

				v8::Locker locker;
				v8::HandleScope handle_scope;
				v8::Context::Scope scope(m_jscontext);

				c->CallEvent(event.press?"keydown":"keyup", keycode2string(event.keycode));
			}
//			std::cout << "KeyPress: " << event.press << " " << event.keycode << std::endl;
		}
		else if(Id == Protocol::LevelConfirm::Id)
		{
			Protocol::LevelConfirm confirm;
			confirm.read(_data);

			// Level was confirmed available by client!
			// Mark them ready.

			Client* c = GetClientById(_from);
			if(c)
			{
				c->m_level_confirmed = true;
			}
		}
		else
		{
			std::cerr << "FATAL: Unhandled world message with id: " << Id << std::endl;
		}
		return false;
	}

	bool PhysWorld::recInit(ZCom_Node *_node, ZCom_ConnID _from,
			   eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool PhysWorld::recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool PhysWorld::recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
				  eZCom_NodeRole _remoterole)
	{
		return false;
	}

	bool PhysWorld::recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid,
					   ZCom_BitStream &_request)
	{
		return false;
	}

	bool PhysWorld::recFileData(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}

	bool PhysWorld::recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}

	bool PhysWorld::recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}


	// Client object member functions:


}
