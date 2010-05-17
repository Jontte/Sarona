#pragma once
#include "StdAfx.h"
#include "Camera.h"
#include "World.h"

namespace Sarona
{
	class Engine
	{
		scoped_ptr<World> m_world;
		scoped_ptr<Camera> m_camera;

		intrusive_ptr<IrrlichtDevice> m_device;

	public:
		Engine(void);
		~Engine(void);

		void CreateWorldFromSPKG(const std::string& filename);
	//#ifdef _DEBUG
		void CreateWorldFromFolder(const std::string& filename);
	//#endif

		void Run();
	};

}