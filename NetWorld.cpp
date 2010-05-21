#include "StdAfx.h"
#include "NetObject.h"
#include "NetWorld.h"

namespace Sarona
{

	NetWorld::NetWorld(IrrlichtDevice* device) : BaseWorld(device)
	{
		// Load script, initialize components
		CreateV8Context();

		// Create camera
		m_camera.reset(new Camera(get_pointer(m_device)));

		scene::ICameraSceneNode * camera = m_device->getSceneManager()->getActiveCamera();
		
		camera->setUpVector(core::vector3df(0,0,1));
		camera->setPosition(core::vector3df(100,50,10));
		//camera->setTarget(core::vector3df(0,0,0));

		m_device->getSceneManager()->setAmbientLight(video::SColor(0,60,60,60));

		m_device->getSceneManager()->addLightSceneNode(	NULL, core::vector3df(50, 30, 310), video::SColorf(1.0f, 1.0f, 1.0f), 300.0f);
	}

	NetWorld::~NetWorld(void)
	{
	}

	void NetWorld::Connect(string host, bool local)
	{
		bool result = this->ZCom_initSockets(true, 0, local);
		if (!result)
		{
			throw std::runtime_error("Unable to create ZCom socket!");
		}

		// Initiate connection
		ZCom_Address server_addr;
		server_addr.setAddress( eZCom_AddressUDP, 0, host.c_str());
		ZCom_ConnID connection_id = this->ZCom_Connect(server_addr, NULL);

		// unable to connect
		// this happens if the connection process can't even be started
		// for some reason
		if (connection_id == ZCom_Invalid_ID)
		{
			throw std::runtime_error("Unable to connect!");
		}
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

		// Set our delta time and time stamp
		u32 TimeStamp = m_device->getTimer()->getTime();
		u32 DeltaTime = 0;

		int mouseprevx = -1;
		int mouseprevy = -1;

		// Hide cursor..
		m_device->getCursorControl()->setVisible(false);

		unsigned long int frame = 0;
		int calc = 0;

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

			// calculate dt
			DeltaTime = m_device->getTimer()->getTime() - TimeStamp;
			TimeStamp = m_device->getTimer()->getTime();

			// Tiukka looppi.

			// Step network code
			this->ZCom_processInput();
			this->ZCom_processOutput();

			if(++calc > 1000.0/DeltaTime )
			{
				calc = 0;
			
				
//				ZCom_ConnStats stats = this->ZCom_getConnectionStats(1);

//				std::cout << "B/W: " << stats.last_sec_in << "/" << stats.last_sec_out << std::endl;
			}

			// Move camera
			m_camera->Step(DeltaTime * 0.001f);

			// Draw everything
			scenemgr->drawAll();

			driver->endScene();

			++frame;
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
			ZCom_requestZoidMode(_id, 1);
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
		if (_result == eZCom_ZoidEnabled)
		{
			std::cout << "Zoidlevel " << _new_level << " entered" << std::endl;
		}
		else
		{
			std::cout << "Request for higher zoid level was denied!" << std::endl;
		}
	}
	void NetWorld::ZCom_cbNodeRequest_Dynamic( ZCom_ConnID _id, ZCom_ClassID _requested_class, ZCom_BitStream *_announcedata,
		eZCom_NodeRole _role, ZCom_NodeID _net_id )
	{
		// We are to create a new dynamic node
		if(BaseWorld::m_objectId == _requested_class)
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
}
