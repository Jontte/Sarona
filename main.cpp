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

	intrusive_ptr<IrrlichtDevice> device;
	
	bool server = (args.size() > 1 && args[1] == "server");
	bool client = (args.size() > 1 && args[1] == "client");

	// if neither specified explicitly, have both for local run
	if(!server && !client)
		server = client = true;

	if(client)
	{
		// Graphical interface
		device = intrusive_ptr<IrrlichtDevice>(
			createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480), 16, false, true, true),
			false /* don't addref */
		);

		
		device->setWindowCaption(L"Sarona");

	}
	else
	{
		// Command line version uses null graphics..
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

		serverworld->LoadLevel("testlevel/");
		serverworld->Bind(9192, (client && server));
	}
	if(client)
	{
		clientworld.reset(new Sarona::NetWorld(get_pointer(device)));

		clientworld->LoadLevel("testlevel/");
		clientworld->Connect("localhost:9192", (client && server));
	}

	if(serverworld)
		serverworld->Start();

	// The main thread has to run the irrlicht loop, apparently..
	if(clientworld)
		clientworld->Loop();

	if(serverworld)
		serverworld->Wait();
}

