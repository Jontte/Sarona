#include "StdAfx.h"
#include "Camera.h"
#include "NetWorld.h"

namespace Sarona
{

	Camera::Camera(IrrlichtDevice* dev, NetWorld* world) 
		: m_device(dev)
		, m_world(world)
		, m_state(CAMERA_FREEFLY)
	{

		m_camera = dev->getSceneManager()->addCameraSceneNode();
		m_device->getSceneManager()->setActiveCamera(m_camera);

		m_camera->bindTargetAndRotation(true);
	}

	Camera::~Camera(void)
	{
	}
	
	void Camera::Translate(int dx, int dy)
	{
		if(m_state != CAMERA_FREEFLY)
			return;

		const float speed = 0.5;
		core::vector3df target = (m_camera->getTarget() - m_camera->getAbsolutePosition());
		core::vector3df relativeRotation; 

		
		relativeRotation.Z = -(f32)(atan2((f64)target.X, (f64)target.Y) * core::RADTODEG64);

		if (relativeRotation.Z < 0)
			relativeRotation.Z += 360;
		if (relativeRotation.Z >= 360)
			relativeRotation.Z -= 360;

		const f64 z1 = core::squareroot(target.X*target.X + target.Y*target.Y);

		relativeRotation.X = (f32)(atan2((f64)z1, (f64)target.Z) * core::RADTODEG64 - 90.0);

		if (relativeRotation.X <= -180)
			relativeRotation.X += 360;
		if (relativeRotation.X > 180)
			relativeRotation.X -= 360;


		relativeRotation.X += dy*speed;
		relativeRotation.Z += dx*speed;

		if(relativeRotation.X > 89)relativeRotation.X = 89;
		if(relativeRotation.X < -89)relativeRotation.X = -89;

		target.set(0,1,0);

		core::matrix4 mat;
			mat.setRotationDegrees(core::vector3df(relativeRotation.X, 0, relativeRotation.Z));
			mat.transformVect(target);

			target.Z *= -1;

		m_camera->setTarget(target + m_camera->getAbsolutePosition());
	}

	void Camera::Step(float dt)
	{
		if(m_state == CAMERA_FREEFLY)
		{
			const float speed = 50.0;

			core::vector3df front = (m_camera->getTarget() - m_camera->getAbsolutePosition());
			front.normalize();

			core::vector3df strafe = front.crossProduct(core::vector3df(0,0,1));
			strafe.normalize();

			core::vector3df move(0,0,0);

			if(m_world->CheckKey(KEY_KEY_A)) move += strafe;
			if(m_world->CheckKey(KEY_KEY_D)) move -= strafe;
			if(m_world->CheckKey(KEY_KEY_W)) move += front;
			if(m_world->CheckKey(KEY_KEY_S)) move -= front;

			m_camera->setTarget(m_camera->getTarget() + move * dt * speed);
			m_camera->setPosition(m_camera->getPosition() + move * dt * speed);
		}
		else if(m_state == CAMERA_FOLLOW)
		{
			// See if object is still valid..

			NetObject * obj = m_world->getById(m_followTarget);

			if(obj)
			{
				// Find position and follow smoothly
				core::vector3df pos = obj->getPosition();

				m_camera->setTarget(pos);

				// Calculate current distance...
				core::vector3df mypos = m_camera->getPosition();

				core::vector3df d = mypos-pos;
				
				d.normalize();

				d *= (float)m_followDistance;

				m_camera->setPosition(pos + d);
			}
			else
			{
				// Enter freefly when we have no ref..
				m_state = CAMERA_FREEFLY;
			}
		}
	}


	void Camera::Follow(NetObject::Id id, double distance)
	{
		m_state = CAMERA_FOLLOW;
		m_followTarget = id;
		m_followDistance = distance;
	}
}
