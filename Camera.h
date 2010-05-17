#pragma once
#include "StdAfx.h"

namespace Sarona
{

	class Camera : IEventReceiver
	{
	private:
		IrrlichtDevice			* m_device;
		scene::ICameraSceneNode * m_camera;	

		bool m_keyPressed [KEY_KEY_CODES_COUNT];

	public:

		void Translate(int dx, int dy);
		void Step(float dt);
		Camera(IrrlichtDevice* dev);
		~Camera();

		// Make the camera follow an existing scene node
		void Follow(scene::ISceneNode* node);

		
		bool OnEvent(const SEvent& event);
	};

}
