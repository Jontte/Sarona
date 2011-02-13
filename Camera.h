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
		double			m_followHeight;
		bool 			m_followFollowPitch;

		// Position mode:
		btVector3 m_pos_begin;
		btVector3 m_pos_end;
		btVector3 m_target_begin;
		btVector3 m_target_end;
		boost::posix_time::ptime m_time_begin;
		boost::posix_time::ptime m_time_end;

		enum {
			CAMERA_CHASE,
			CAMERA_POSITION,
			CAMERA_FREE
		} m_state;

	public:

		void Translate(int dx, int dy);
		void Step(float dt);
		Camera(IrrlichtDevice* dev, NetWorld* world);
		~Camera();

		// Make the camera follow an existing NetObject
		void SetCameraChase(NetObject::Id id, double distance, double height, bool follow_pitch);
		void SetCameraPos(btVector3 pos_begin, btVector3 pos_end, btVector3 target_begin, btVector3 target_end, double time);
		void SetCameraFree();

		bool OnEvent(const SEvent& event);
	};

}
#endif
