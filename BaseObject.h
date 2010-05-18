#pragma once
#include "StdAfx.h"

namespace Sarona
{
class BaseObject
{
protected:
	// Irrlicht device
	IrrlichtDevice*						m_device;
	// Irrlicht node
	scene::ISceneNode*					m_scenenode;
public:
	BaseObject(IrrlichtDevice* device, scene::ISceneNode* node);
	virtual ~BaseObject();


};
}