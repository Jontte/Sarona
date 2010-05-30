#pragma once
#include "StdAfx.h"

namespace Sarona
{

class BaseWorld 
	: public ZCom_Control
	, public ZCom_Node
	, public ZCom_NodeEventInterceptor
{
protected:
	// Level config
	std::string m_name;
	std::string m_description;
	std::string m_author;

	// V8
	v8::Persistent<v8::Context>		m_jscontext;

	// Irrlicht
	intrusive_ptr<IrrlichtDevice>	m_device;

	// Networking
	ZCom_Control*					m_control;

	void RegisterZComObjects();

public:

	static int m_objectId;
	static int m_commId;

	void LoadLevel(std::string level);

	BaseWorld(IrrlichtDevice* device);

	virtual ~BaseWorld(void);
};

}