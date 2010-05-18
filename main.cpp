#include "StdAfx.h"

#include "Engine.h"

#include <iostream>

#include <windows.h>

#include <boost/archive/text_oarchive.hpp>
#include "PacketDef.h"
#include <fstream>

int main()
{
	Sarona::Protocol::VersionCheck kissa;

	kissa.major = 5;
	kissa.minor = 2;
	
	std::ofstream fout("kissa.out", std::ios::binary);
	boost::archive::text_oarchive arch(fout);

	arch << kissa;

	fout.close();

	AllocConsole();

	Sarona::Engine engine;
	
	engine.LoadFolder("testlevel/");

	engine.SetupLocal();

	engine.Run();
}

