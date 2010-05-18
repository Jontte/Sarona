#pragma once
#include "StdAfx.h"
#include "Camera.h"
#include "BaseWorld.h"
#include "NetworkCtx.h"

namespace Sarona
{
	class Engine
	{
		scoped_ptr<BaseWorld> m_world;
		scoped_ptr<Camera> m_camera;
		scoped_ptr<NetworkCtx> m_netctx;

		intrusive_ptr<IrrlichtDevice> m_device;

	public:
		Engine(void);
		~Engine(void);

		bool LoadZip(const std::string& filename);
		bool LoadFolder(const std::string& filename);

		void SetupLocal();
		void SetupServer(int port, int remoteplayers);
		void SetupClient(std::string hostname, int port);

		void Run();
	};

}