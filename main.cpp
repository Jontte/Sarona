#include "StdAfx.h"

#include "Engine.h"
#include "World.h"

#include <iostream>

#include <windows.h>

int main()
{
	AllocConsole();

	Sarona::Engine engine;
	
	engine.CreateWorldFromFolder("testlevel/");

	engine.Run();
}

