#pragma once
#include "StdAfx.h"

namespace Sarona
{

	class NetworkCtx
	{
	private:
		NetworkCtx(void);
	public:

		static NetworkCtx* CreateHost(int port, int remoteplayers);
		static NetworkCtx* CreateClient(std::string hostname, int port);

		~NetworkCtx(void);
	};

}