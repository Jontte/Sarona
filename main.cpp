#include "StdAfx.h"
#include "Util.h"

#include "PhysWorld.h"
#include "NetWorld.h"

#include <iostream>
#include <windows.h>

int main(int argc, char *argv[])
{
	AllocConsole();
	vector<string> args(argv,argv+argc);

	// initialize Zoidcom
	scoped_ptr<ZoidCom> zcom(new ZoidCom());

	if (!zcom || !zcom->Init())
	{
		throw std::runtime_error("Unable to initialize ZoidCom!");
	}

	zcom->setLogLevel(2);

	intrusive_ptr<IrrlichtDevice> device;
	

	enum {
		DEDICATED_SERVER,
		JOIN_GAME,
		HOST_AND_PLAY,
		LOCAL_GAME
	} gametype;

	gametype = LOCAL_GAME; // Default

	if(args.size() > 1 && args[1] == "server")
		gametype = DEDICATED_SERVER;
	if(args.size() > 1 && args[1] == "client")
		gametype = JOIN_GAME;
	if(args.size() > 1 && args[1] == "hostandplay")
		gametype = HOST_AND_PLAY;

	// whether to run server here
	bool server = 
		(gametype == DEDICATED_SERVER) ||
		(gametype == HOST_AND_PLAY) || 
		(gametype == LOCAL_GAME);

	// whether to run client here
	bool client = 
		(gametype == JOIN_GAME) || 
		(gametype == HOST_AND_PLAY) || 
		(gametype == LOCAL_GAME);

	if(gametype != DEDICATED_SERVER)
	{
		// Graphical interface needed
		device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800,600), 16, false, true, true),
			false /* don't addref */
		);
		device->setWindowCaption(L"Sarona");
	}
	else
	{
		// Dedicated server uses null graphics..
		device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_NULL, core::dimension2d<u32>(640,480), 16, false, true, true),
			false /* don't addref */
		);
	}

	device->getLogger()->setLogLevel(ELL_INFORMATION);

	scoped_ptr<Sarona::PhysWorld> serverworld;
	scoped_ptr<Sarona::NetWorld> clientworld;

	if(server)
	{
		serverworld.reset(new Sarona::PhysWorld(get_pointer(device)));

		serverworld->SetLevel("testlevel/");
		serverworld->Bind(9192, (gametype == LOCAL_GAME));
	}
	if(client)
	{
		clientworld.reset(new Sarona::NetWorld(get_pointer(device)));

		clientworld->SetLevel("testlevel/");
		clientworld->Connect("localhost:9192", (gametype == LOCAL_GAME));
	}

	if(serverworld)
		serverworld->Start();

	// The main thread has to run the irrlicht loop, apparently..
	if(clientworld)
		clientworld->Loop();

	if(serverworld)
		serverworld->Wait();
}

