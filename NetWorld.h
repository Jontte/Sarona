#pragma once
#include "BaseWorld.h"

namespace Sarona
{

	class NetWorld : public BaseWorld
	{
	private:
		void CreateV8Context();
	
	public:
		NetWorld(IrrlichtDevice* device, NetworkCtx* ctx);
		~NetWorld(void);

		void Start();
		void Step(float dt);
	};

}