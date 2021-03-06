#include "StdAfx.h"
#include "NetObject.h"
#include "NetWorld.h"
#include "PacketDef.h"

namespace Sarona
{

	NetWorld::NetWorld(IrrlichtDevice* device) : BaseWorld<NetWorld, NetObject>(device)
	{
		ZCom_setUpstreamLimit(0, 0);

		this->beginReplicationSetup(0);
		this->endReplicationSetup();
		this->setEventInterceptor(this);
		this->registerNodeUnique(TypeRegistry::m_commId, eZCom_RoleOwner, this);


		// Create camera
		m_camera.reset(new Camera(get_pointer(m_device), this));

		scene::ICameraSceneNode * camera = m_device->getSceneManager()->getActiveCamera();

		camera->setUpVector(core::vector3df(0,0,1));
		camera->setPosition(core::vector3df(100,50,10));
		//camera->setTarget(core::vector3df(0,0,0));

		m_device->getSceneManager()->setAmbientLight(video::SColor(0,60,60,60));

		m_device->getSceneManager()->addLightSceneNode(	NULL, core::vector3df(50, 30, 310), video::SColorf(1.0f, 1.0f, 1.0f), 300.0f);

		gui::IGUIEnvironment* env = m_device->getGUIEnvironment();
//		gui::IGUISkin* skin = env->getSkin();
		//skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);

		// Create textbox..
		m_device->getGUIEnvironment()->addStaticText(L"Sarona", core::rect<s32>(64,64,64+64*4,64+32), true, true);

		m_device->setEventReceiver(this);

		// Create ground

	/*	scene::IMeshSceneNode * node =
		m_device->getSceneManager()->addMeshSceneNode(
			m_device->getSceneManager()->addHillPlaneMesh("groundmesh", core::dimension2d<f32>(10,10), core::dimension2d<u32>(100,100)), NULL, -1, core::vector3df(0,0,0), core::vector3df(90,0,0)
			);
*/
		//node -> setMaterialTexture(0, m_device->getVideoDriver()->getTexture("saronacube.png"));
	}

	NetWorld::~NetWorld(void)
	{
	}

	bool NetWorld::OnEvent(const SEvent & event )
	{
		// Handle keypress events

		if(event.EventType == EET_KEY_INPUT_EVENT)
		{
			UpdateKeyState(event.KeyInput.Key, event.KeyInput.PressedDown);

			// Let the event propagate, though.
			return false;
		}
		return false;
	}

	void NetWorld::UpdateKeyState(u8 keycode, bool pressed)
	{
		// Update key press statuses using a large array for the bits..

		if(m_keystate[keycode] != pressed)
		{
			m_keystate[keycode] = pressed;

			// Send ALL keypresses to server..
			ZCom_BitStream * data = new ZCom_BitStream;
				Protocol::KeyPressEvent keypress;
				keypress.keycode = keycode;
				keypress.press = pressed;
				keypress.write(*data);
			this->sendEventDirect(eZCom_ReliableOrdered, data, m_serverConnectionId);

		}
	}
	bool NetWorld::CheckKey(u8 keycode)
	{
		return m_keystate[keycode];
	}

	void NetWorld::Connect(string host, bool local)
	{
		bool result = this->ZCom_initSockets(!local, 0, 1);
		if (!result)
		{
			throw std::runtime_error("Unable to create ZCom socket!");
		}

		// Initiate connection
		ZCom_Address server_addr;
		if(local)
		{
			server_addr.setAddress( eZCom_AddressLocal, 0, host.c_str());
		}
		else
		{
			server_addr.setAddress( eZCom_AddressUDP, 0, host.c_str());
		}
		ZCom_ConnID connection_id = this->ZCom_Connect(server_addr, NULL);

		// unable to connect
		// this happens if the connection process can't even be started
		// for some reason
		if (connection_id == ZCom_Invalid_ID)
		{
			throw std::runtime_error("Unable to connect!");
		}

		m_serverConnectionId = connection_id;
	}

	void NetWorld::CreateV8Context()
	{
		v8::Locker locker;
		v8::HandleScope handle_scope;

		m_jscontext = v8::Context::New();

		v8::Locker::StartPreemption(50);
	}

	void NetWorld::Loop()
	{
		video::IVideoDriver* driver = m_device->getVideoDriver();
		scene::ISceneManager* scenemgr = m_device->getSceneManager();

		// Wait for game start event
		while(m_device->run())
		{
			driver->beginScene(true, true, video::SColor(255, 128, 160, 255));

			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();

			// Draw everything
			scenemgr->drawAll();

			driver->endScene();

			if(m_gamerunning)
				break;
		}
		if(!m_gamerunning)
		{
			// Lobby was quit prematurely.
			return;
		}

		// Create v8 context
		CreateV8Context();
		// Load script, initialize components
		LoadLevel(false); // false = don't run scripts (we're not the host)

		// Set our delta time and time stamp
		SimpleTimer timer;

		int mouseprevx = -1;
		int mouseprevy = -1;

		// Hide cursor..
		m_device->getCursorControl()->setVisible(false);

		unsigned long int frame = 0;
		unsigned long int framecounter = 0;

		double frametime = 1.0/50;

		while(m_device->run())
		{
			driver->beginScene(true, true, video::SColor(255, 128, 160, 255));

			// manage mouse
			if(m_device->isWindowFocused())
			{
				int currentx = m_device->getCursorControl()->getPosition().X;
				int currenty = m_device->getCursorControl()->getPosition().Y;

				int dx = currentx - mouseprevx;
				int dy = currenty - mouseprevy;

				if(mouseprevx != -1)
					m_camera->Translate(dx, dy);

				if(dx != 0 || dy != 0)
					m_device->getCursorControl()->setPosition(0.5f, 0.5f);
			}
			mouseprevx = m_device->getCursorControl()->getPosition().X;
			mouseprevy = m_device->getCursorControl()->getPosition().Y;

			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();
			//this->ZCom_processReplicators(DeltaTime);

			// Move camera
			m_camera->Step(frametime);

			// Remove objects scheduled for deletion
			RemoveZombies();

			// Refresh object positions
			const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::local_time();
			for(unsigned i = 0; i < m_objects.size(); i++)
			{
				m_objects[i].RefreshPosRot(now);
			}

			// Draw everything
			scenemgr->drawAll();
			driver->endScene();

			++frame;
			++framecounter;
			double elapsed = timer.elapsed();
			if(elapsed > 1.0)
			{
				timer.restart();
				frametime = elapsed/framecounter;
				framecounter = 0;
				ZCom_ConnStats stats = this->ZCom_getConnectionStats(1);

	//			stats.
//				std::cout << "In/Out pps: " << stats.current_inp << "/" << stats.current_outp << " Client FPS: " << driver->getFPS() << ", " << 1.0/frametime << std::endl;
				std::cout << "In/Out pps: " << stats.last_sec_inp << "/" << stats.last_sec_outp<< " Client FPS: " << driver->getFPS() << " loss: " << (int)stats.last_sec_loss_percent << "% ping: " << stats.avg_ping << "ms" << std::endl;

				core::stringw str = L"Sarona [";
				str += driver->getName();
				str += L"] FPS: ";
				str += (s32)driver->getFPS();

				m_device->setWindowCaption(str.c_str());
			}
		}
	}

	void NetWorld::RemoveZombies()
	{
		// Remove objects with the 'deleteme' bit set
		ptr_vector<NetObject>::iterator iter = m_objects.begin();
		while(iter != m_objects.end())
		{
			if(iter->IsZombie())
			{
				iter = m_objects.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	bool NetWorld::ZCom_cbConnectionRequest(ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply)
	{
		return true; // Allow incoming connection
	}
	void NetWorld::ZCom_cbConnectionSpawned( ZCom_ConnID _id )
	{
	}
	void NetWorld::ZCom_cbConnectionClosed( ZCom_ConnID _id, eZCom_CloseReason _reason, ZCom_BitStream &_reasondata )
	{

	}
	void NetWorld::ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply )
	{
		if (_result == eZCom_ConnAccepted)
		{
			ZCom_requestZoidMode(_id, zU16(1));
			ZCom_requestDownstreamLimit(_id, (zU16)50, (zU16)65535 );
		}
		else
		{
			// connection failed
			std::cout << "Connection failed!" << std::endl;
		}
	}
	void NetWorld::ZCom_cbDataReceived( ZCom_ConnID _id, ZCom_BitStream &_data )
	{

	}
	bool NetWorld::ZCom_cbZoidRequest( ZCom_ConnID _id, zU8 _requested_level, ZCom_BitStream &_reason )
	{
		return false;
	}
	void NetWorld::ZCom_cbZoidResult( ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason )
	{
/*		if (_result == eZCom_ZoidEnabled)
		{
			std::cout << "Zoidlevel " << _new_level << " entered" << std::endl;
		}
		else
		{
			std::cout << "Request for higher zoid level was denied!" << std::endl;
		}*/
	}
	void NetWorld::ZCom_cbNodeRequest_Dynamic( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
		eZCom_NodeRole _role, ZCom_NodeID _net_id )
	{
		// We are to create a new dynamic node
		if(TypeRegistry::m_objectId == _requested_class)
		{
			// which is an object
			NetObject * obj = new NetObject(this, get_pointer(m_device));
			m_objects.push_back(obj);
		}
	}
	void NetWorld::ZCom_cbNodeRequest_Tag( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
		eZCom_NodeRole _role, zU32 _tag )
	{

	}
	bool NetWorld::ZCom_cbDiscoverRequest( const ZCom_Address &_addr, ZCom_BitStream &_request,
		ZCom_BitStream &_reply )
	{
		return false;
	}
	void NetWorld::ZCom_cbDiscovered( const ZCom_Address & _addr, ZCom_BitStream &_reply )
	{

	}

	// Node events

	bool NetWorld::recUserEvent(ZCom_Node *_node, ZCom_ConnID _from,
					eZCom_NodeRole _remoterole, ZCom_BitStream &_data,
					zU32 _estimated_time_sent)
	{
		unsigned int Id = _data.getInt(16);

		if(Id == Protocol::GameStartNotify::Id)
		{
			Protocol::GameStartNotify notify;
			notify.read(_data); // has to be called
			m_gamerunning = true;
		}
		else if(Id == Protocol::LevelSelect::Id)
		{
			Protocol::LevelSelect select;
			select.read(_data);

			SetLevel(select.name);

			// TODO: always accept the level

			Protocol::LevelConfirm confirm;
			ZCom_BitStream * stream = new ZCom_BitStream;
			confirm.write(*stream);
			this->sendEventDirect(eZCom_ReliableOrdered, stream, _from);
		}
		else if(Id == Protocol::SetCameraChase::Id)
		{
			Protocol::SetCameraChase chase;
 			chase.read(_data);
			if(m_camera)
				m_camera->SetCameraChase(chase.nodeid, chase.distance, chase.height, chase.follow_pitch);
		}
		else if(Id == Protocol::SetCameraPos::Id)
		{
			Protocol::SetCameraPos position;
 			position.read(_data);
			if(m_camera)
				m_camera->SetCameraPos(
					position.position_begin,
					position.position_end,
					position.lookat_begin,
					position.lookat_end,
					position.seconds
				);
		}
		return false;
	}

	bool NetWorld::recInit(ZCom_Node *_node, ZCom_ConnID _from,
			   eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool NetWorld::recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool NetWorld::recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
				  eZCom_NodeRole _remoterole)
	{
		return false;
	}

	bool NetWorld::recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid,
					   ZCom_BitStream &_request)
	{
		return false;
	}

	bool NetWorld::recFileData(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}

	bool NetWorld::recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}

	bool NetWorld::recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}
}
