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
	string m_name;
	string m_description;
	string m_author;

	string m_levelname;

	// V8
	v8::Persistent<v8::Context>		m_jscontext;

	// Irrlicht
	intrusive_ptr<IrrlichtDevice>	m_device;

	// Networking
	ZCom_Control*					m_control;

	void RegisterZComObjects();

	void LoadLevel(bool runscripts);

	// Whether the game (physical simulation & friends) is running
	bool m_gamerunning;

public:

	static int m_objectId;
	static int m_commId;

	void SetLevel(std::string level);

	BaseWorld(IrrlichtDevice* device);

	virtual ~BaseWorld(void);
};

}