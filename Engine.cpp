#include "StdAfx.h"
#include "Engine.h"

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
		createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480), 16, false, true, true),
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

	m_device->getSceneManager()->addLightSceneNode(	NULL, core::vector3df(50, 30, 70), video::SColorf(1.0f, 1.0f, 1.0f), 100.0f);
}

Engine::~Engine(void)
{
	m_camera.reset();
}

void Engine::Run()
{
	if(!get_pointer(m_world))
		return;

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

void Engine::CreateWorldFromFolder(const std::string& filename)
{
	bool success = m_device->getFileSystem()->addFileArchive(
		filename.c_str(),
		false, /* ignore case */
		false, /* ignore paths */
		io::EFAT_FOLDER
		);
	if(success)
	{
		std::cout << "Success!" << std::endl;
		m_world.reset(new World(m_device));
	}
	else
	{
		std::cout << "FAIL! " << m_device->getFileSystem()->getWorkingDirectory().c_str() << std::endl;
	}
}


}