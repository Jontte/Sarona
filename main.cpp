#include "StdAfx.h"
#include "Util.h"

#include "PhysWorld.h"
#include "NetWorld.h"

#include "GameMenu.h"

#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif

void zoidcom_logger(const char *_log) {
	printf("ZCOM: %s\n", _log);
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
	AllocConsole();
#endif
	vector<string> args(argv,argv+argc);

	// initialize Zoidcom
	scoped_ptr<ZoidCom> zcom(new ZoidCom(zoidcom_logger));

	if (!zcom || !zcom->Init())
	{
		throw std::runtime_error("Unable to initialize ZoidCom!");
	}

	zcom->setLogLevel(0);

	intrusive_ptr<IrrlichtDevice> device;

	if(args.size() > 1 && args[1] == "server")
	{
		// Skip graphical interface
		// Dedicated server uses 'null' graphics device..
		device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_NULL, core::dimension2d<u32>(640,480), 16, false, true, true),
			false /* don't addref */
		);
		device->getLogger()->setLogLevel(ELL_INFORMATION);
		addHeightMapLoader(get_pointer(device));

		// Start server, run game instances in loop
		while(true)
		{
			std::cout << "Loading server instance...";

			scoped_ptr<Sarona::PhysWorld> serverworld(
				new Sarona::PhysWorld(get_pointer(device))
			);

			serverworld->SetLevel("testlevel/");
			serverworld->Bind(9192, false);

			serverworld->Start();
			serverworld->Wait();
		}
	}
	else
	{
		// Display main menu, graphical interface needed
		device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800,600), 16, false, true, true),
			false /* don't addref */
		);
		device->getLogger()->setLogLevel(ELL_WARNING);
		addHeightMapLoader(get_pointer(device));

		Sarona::GameMenu menu(get_pointer(device));

		bool local_game = true;
		bool server = true;

		// Run the main menu, let user decide what to do
		//menu.Run(local_game, server);

		scoped_ptr<Sarona::NetWorld> clientworld;
		scoped_ptr<Sarona::PhysWorld> serverworld;

		if(server)
		{
			serverworld.reset(new Sarona::PhysWorld(get_pointer(device)));

			serverworld->SetLevel("testlevel/");
			serverworld->Bind(9192, local_game);
		}

		clientworld.reset(new Sarona::NetWorld(get_pointer(device)));
		clientworld->SetLevel("testlevel/");
		clientworld->Connect("localhost:9192", local_game);

		if(serverworld)
			serverworld->Start();

		// The main thread has to run the irrlicht loop, apparently..
		clientworld->Loop();

		if(serverworld)
		{
			serverworld->Shutdown();
			serverworld->Wait();
		}
	}
	return 0;
}

