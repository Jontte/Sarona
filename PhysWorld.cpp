#include "StdAfx.h"
#include "PhysWorld.h"
#include "JSObject.h"
#include "Util.h"
#include "PacketDef.h"

#include <stdexcept>

namespace Sarona
{
	PhysWorld::PhysWorld(IrrlichtDevice * dev)
		: BaseWorld(dev)
	{
		ZCom_setUpstreamLimit(100000, 100000);

		this->beginReplicationSetup(0);
		this->endReplicationSetup();
		this->setEventInterceptor(this);
		this->registerNodeUnique(m_commId, eZCom_RoleAuthority, this);
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
		// Get rid of any existing contexts..
		m_jscontext.Dispose();

		v8::Locker locker;

		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
		global->SetInternalFieldCount(2);

		// Scene
		v8::Handle<v8::ObjectTemplate> scene = v8::ObjectTemplate::New();
		scene->SetInternalFieldCount(1);

		// Static functions
		global->Set(v8::String::New("print"), v8::FunctionTemplate::New(PhysWorld::JSPrint));

		// Objects
		global->Set(v8::String::New("scene"), scene);

		CProxyV8::ProxyClass<JSObject>* pJSObject = CProxyV8::ProxyClass<JSObject>::instance()->init("Object");
		pJSObject->Expose(&JSObject::push, "push");
		global->Set(v8::String::New("Object"), pJSObject->GetFunctionTemplate());

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
	PhysWorld::Client* PhysWorld::GetClientById(const ZCom_ConnID& id)
	{
		ptr_vector<Client>::iterator iter = m_clients.begin();
		while(iter != m_clients.end())
		{
			if(iter->connection_id == id)
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
			this->sendEventDirect(eZCom_ReliableOrdered, stream.Duplicate(), iter->connection_id);
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
			v8::Local<v8::ObjectTemplate> tpl = v8::ObjectTemplate::New();
			tpl->Set(v8::String::New("bind"), v8::FunctionTemplate::New(PhysWorld::JSPlayerBind,v8::Object::New()));
			tpl->SetInternalFieldCount(1);

			v8::Local<v8::Object> playerobj = tpl->NewInstance();
			playerobj->Set(v8::String::New("foo"), v8::String::New("bar"));
		
			v8::Handle<v8::External> external_obj = v8::External::New(&m_clients[i]);
			playerobj->SetInternalField(0, external_obj);

			players->Set(i, playerobj);
		}
		obj->Set(v8::String::New("players"), players);

		// Call 'level_start' defined in script

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
	}

	void PhysWorld::Loop()
	{
		// Wait until enough players have joined...
		while(true)
		{
			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();

			boost::this_thread::sleep(boost::posix_time::milliseconds(50));

			if(!m_gamerunning && m_clients.size() > 0)
			{
				// Wait for enough confirmations to start the game
				int confirmations = 0;
				ptr_vector<Client>::iterator iter = m_clients.begin();
				while(iter != m_clients.end())
				{
					if(iter->level_confirmed)
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

						if(timewaited > boost::posix_time::seconds(2))
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

	v8::Handle<v8::Value> PhysWorld::PlayerBind(const v8::Arguments& args)
	{
		// Bind event handling function.

		if(args.Length() != 3)
			return v8::ThrowException(v8::String::New("Invalid number of arguments passed to player.bind"));
		if(!args[0]->IsString())
			return v8::ThrowException(v8::String::New("Invalid type"));
		if(!args[1]->IsString())
			return v8::ThrowException(v8::String::New("Invalid type"));
		if(!args[2]->IsFunction())
			return v8::ThrowException(v8::String::New("Invalid type"));


		// Convert to Native types
	
		string eventtype(*v8::String::Utf8Value(args[0]->ToString()));
		string eventname(*v8::String::Utf8Value(args[1]->ToString()));
		v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(args[2]);
	
		// Find Client object.

		Client* c = static_cast<Client*>(v8::Handle<v8::External>::Cast(args.Holder()->GetInternalField(0))->Value());
		c->bind_event(eventtype, eventname, function, args.Holder());

		c->call_event(eventtype, eventname);

		return v8::Undefined();
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
		// Allow incoming connection when game is not running (we're in lobby)
		return !m_gamerunning; 
	}
	void PhysWorld::ZCom_cbConnectionSpawned( ZCom_ConnID _id )
	{
		ZCom_requestDownstreamLimit(_id, (zU16)1000, (zU16)100000 );
		//ZCom_simulateLoss(_id, 0.5);
	}
	void PhysWorld::ZCom_cbConnectionClosed( ZCom_ConnID _id, eZCom_CloseReason _reason, ZCom_BitStream &_reasondata )
	{
		// Mark client disconnected
		Client *c = GetClientById(_id);
		if(c)
		{
			c->disconnected = true;
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
			Client * c = new Client;
			c->connection_id = _id;
			c->disconnected = false ; // They're active
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
				
				v8::Locker locker;
				v8::HandleScope handle_scope;
				v8::Context::Scope scope(m_jscontext);

				c->call_event(event.press?"keydown":"keyup", keycode2string(event.keycode));
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
				c->level_confirmed = true;
			}
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

	
	void PhysWorld::Client::bind_event(const string& eventtype, const string& eventname, 
		v8::Handle<v8::Function> func, v8::Handle<v8::Object> context)
	{
		if(eventtype == "" || eventname == "") return;

		// Register event
		//TODO: doesn't handle duplicates
		Event * e = new Event;
		e->eventtype = eventtype;
		e->eventname = eventname;
		e->function = v8::Persistent<v8::Function>::New(func);
		e->context = v8::Persistent<v8::Object>::New(context);
		m_events.push_back(e);
	}
	void PhysWorld::Client::call_event(const string& eventtype, const string& eventname, std::vector<v8::Handle< v8::Value > >& args)
	{
		// Call event
		if(eventtype == "" || eventname == "") return;

		ptr_vector<Event>::iterator iter = m_events.begin();
		while(iter != m_events.end())
		{
			if(iter->eventtype == eventtype && 
				iter->eventname == eventname)
			{
				v8::TryCatch trycatch;
		
//				v8::Handle<v8::Value> result = iter->function->Call(iter->context, 0, NULL);
				v8::Handle<v8::Value> result = iter->function->Call(v8::Context::GetCurrent()->Global(), args.size(), args.empty()?NULL:&args[0]);
				if(result.IsEmpty())
				{
					v8::Handle<v8::Value> exception = trycatch.Exception();
					v8::Handle<v8::Message> message = trycatch.Message();

					v8::String::AsciiValue exception_str(exception);

					std::cout << "Exception: " << *exception_str << std::endl;
					std::cout << "	: " << *v8::String::AsciiValue(message->GetSourceLine()) << std::endl;
				}
			}
			iter++;
		}
	}
}