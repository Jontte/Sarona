#include "StdAfx.h"
#include "Engine.h"

#include <iostream>
#include <windows.h>

int main()
{
	AllocConsole();

	Sarona::Engine engine;
	
	engine.LoadFolder("testlevel/");

	engine.SetupLocal();

	engine.Run();
}

