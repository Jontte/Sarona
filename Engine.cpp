#include "StdAfx.h"
#include "Engine.h"
#include "PhysWorld.h"
#include "NetWorld.h"
#include "NetworkCTX.h"

void intrusive_ptr_add_ref(IReferenceCounted* t)
{
	if(t)
		t->grab();
}
void intrusive_ptr_release(IReferenceCounted* t)
{
	if(t)
		t->drop();
}


namespace Sarona
{

	Engine::Engine(void)
	{
		// Create irrlicht device
		m_device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_DIRECT3D8, core::dimension2d<u32>(640,480), 16, false, true, true),
			false /* don't addref */
		);

		m_device->setWindowCaption(L"Sarona");

		// Create camera
		m_camera.reset(new Camera(get_pointer(m_device)));

		scene::ICameraSceneNode * camera = m_device->getSceneManager()->getActiveCamera();
		
		camera->setUpVector(core::vector3df(0,0,1));
		camera->setPosition(core::vector3df(100,50,10));
		//camera->setTarget(core::vector3df(0,0,0));


		m_device->getSceneManager()->setAmbientLight(video::SColor(0,60,60,60));

		m_device->getSceneManager()->addLightSceneNode(	NULL, core::vector3df(50, 30, 310), video::SColorf(1.0f, 1.0f, 1.0f), 300.0f);
	}

	Engine::~Engine(void)
	{
		m_camera.reset();
		m_world.reset(); // make sure world resets before irrlicht device
	}

	void Engine::Run()
	{
		if(!get_pointer(m_world))
			return;

		m_world->LoadLevel();
		m_world->Start();

		video::IVideoDriver* driver = m_device->getVideoDriver();
		scene::ISceneManager* scenemgr = m_device->getSceneManager();

		// Set our delta time and time stamp
		u32 TimeStamp = m_device->getTimer()->getTime();
		u32 DeltaTime = 0;

		int mouseprevx = -1;
		int mouseprevy = -1;

		while(m_device->run())
		{
			driver->beginScene(true, true, video::SColor(255, 128, 160, 255));

			// manage mouse

			int currentx = m_device->getCursorControl()->getPosition().X;
			int currenty = m_device->getCursorControl()->getPosition().Y;

			int dx = currentx - mouseprevx;
			int dy = currenty - mouseprevy;

			if(mouseprevx != -1)
				m_camera->Translate(dx, dy);

			if(dx != 0 || dy != 0)
				m_device->getCursorControl()->setPosition(0.5f, 0.5f);

			mouseprevx = m_device->getCursorControl()->getPosition().X;
			mouseprevy = m_device->getCursorControl()->getPosition().Y;


			// calculate dt
			DeltaTime = m_device->getTimer()->getTime() - TimeStamp;
			TimeStamp = m_device->getTimer()->getTime();


			m_world->Step(DeltaTime * 0.001f);
			m_camera->Step(DeltaTime * 0.001f);
			scenemgr->drawAll();

			driver->endScene();
		}
	}

	bool Engine::LoadFolder(const std::string& filename)
	{
		return m_device->getFileSystem()->addFileArchive(
			filename.c_str(),
			false, /* ignore case */
			false, /* ignore paths */
			io::EFAT_FOLDER
			);
	}
	bool Engine::LoadZip(const std::string& filename)
	{
		return m_device->getFileSystem()->addFileArchive(
			filename.c_str(),
			false, /* ignore case */
			false, /* ignore paths */
			io::EFAT_ZIP
			);
	}


	void Engine::SetupLocal()
	{
		m_world.reset(new PhysWorld(get_pointer(m_device)));
	}

	void Engine::SetupServer(int port, int remoteplayers)
	{
		m_netctx.reset(NetworkCtx::CreateHost(port, remoteplayers));

		m_world.reset(new PhysWorld(get_pointer(m_device), get_pointer(m_netctx)));
	}
	void Engine::SetupClient(std::string hostname, int port)
	{
		m_netctx.reset(NetworkCtx::CreateClient(hostname, port));

		m_world.reset(new NetWorld(get_pointer(m_device), get_pointer(m_netctx)));
	}
}