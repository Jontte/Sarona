#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_
#include "StdAfx.h"
#include "NetObject.h"

namespace Sarona
{
	class NetWorld;

	class Camera
	{
	private:
		IrrlichtDevice			* m_device;
		scene::ICameraSceneNode * m_camera;
		NetWorld				* m_world;

		// Follow mode:
		NetObject::Id	m_followTarget;
		double			m_followDistance;

		enum {
			CAMERA_FOLLOW,
			CAMERA_FREEFLY,
			CAMERA_TRANSITION
		} m_state;

	public:

		void Translate(int dx, int dy);
		void Step(float dt);
		Camera(IrrlichtDevice* dev, NetWorld* world);
		~Camera();

		// Make the camera follow an existing NetObject
		void Follow(NetObject::Id id, double distance);

		bool OnEvent(const SEvent& event);
	};

}
#endif
