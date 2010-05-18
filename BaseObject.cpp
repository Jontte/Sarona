#include "StdAfx.h"
#include "BaseObject.h"

namespace Sarona
{
BaseObject::BaseObject(IrrlichtDevice* device, scene::ISceneNode* node)
	: m_device(device)
	, m_scenenode(node)
{
}

BaseObject::~BaseObject()
{
}
}