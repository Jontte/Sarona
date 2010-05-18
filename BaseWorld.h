#pragma once
#include "StdAfx.h"
#include "BaseObject.h"
#include "NetworkCtx.h"

namespace Sarona
{

class BaseWorld
{
protected:
	// Level config
	std::string m_name;
	std::string m_description;
	std::string m_author;

	// V8
	v8::Persistent<v8::Context>		m_jscontext;

	// Irrlicht 
	IrrlichtDevice*					m_device;

	// General
	ptr_vector<BaseObject>			m_objects;

	// Networking
	scoped_ptr<NetworkCtx>			m_netctx;

public:

	void LoadLevel();

	virtual void Start() = 0;
	virtual void Step(float dt) = 0;

	BaseWorld(IrrlichtDevice* device, NetworkCtx * netctx);

	virtual ~BaseWorld(void);
};

}