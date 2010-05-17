#include "StdAfx.h"
#include "Camera.h"

namespace Sarona
{

	Camera::Camera(IrrlichtDevice* dev) : m_device(dev)
	{

		m_camera = dev->getSceneManager()->addCameraSceneNode();
		m_device->getSceneManager()->setActiveCamera(m_camera);

		m_camera->bindTargetAndRotation(true);

		m_device->setEventReceiver(this);


		for(int i = 0; i < KEY_KEY_CODES_COUNT; i++)
			m_keyPressed [i] = false;
	}

	Camera::~Camera(void)
	{
	}
	
	void Camera::Translate(int dx, int dy)
	{
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

	bool Camera::OnEvent(const SEvent& event)
	{
		switch(event.EventType) 
		{ 
			case EET_KEY_INPUT_EVENT: 
				m_keyPressed[event.KeyInput.Key] = event.KeyInput.PressedDown;
				return true;
			break; 
		}
		return false;		
	}

	void Camera::Step(float dt)
	{
		const float speed = 50.0;

		core::vector3df front = (m_camera->getTarget() - m_camera->getAbsolutePosition());
		front.normalize();

		core::vector3df strafe = front.crossProduct(core::vector3df(0,0,1));
		strafe.normalize();

		core::vector3df move(0,0,0);

		if(m_keyPressed[KEY_KEY_A]) move += strafe;
		if(m_keyPressed[KEY_KEY_D]) move -= strafe;
		if(m_keyPressed[KEY_KEY_W]) move += front;
		if(m_keyPressed[KEY_KEY_S]) move -= front;

		m_camera->setTarget(m_camera->getTarget() + move * dt * speed);
		m_camera->setPosition(m_camera->getPosition() + move * dt * speed);
	}
}
