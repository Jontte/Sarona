#include "StdAfx.h"
#include "NetworkCTX.h"
#include "PacketDef.h"

namespace Sarona
{

	NetworkCtx::NetworkCtx(void)
	{

	}

	NetworkCtx::~NetworkCtx(void)
	{

	}

	NetworkCtx* NetworkCtx::CreateHost(int port, int remoteplayers)
	{
		// host game, etc.

		return NULL;
	}
	NetworkCtx* NetworkCtx::CreateClient(std::string host, int port)
	{
		// join game, etc.

		return NULL;
	}
}
