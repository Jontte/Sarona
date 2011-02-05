#include "StdAfx.h"
#include "JSObject.h"
#include "JSConvert.h"

#include "PhysWorld.h"

namespace Sarona
{
#define THROW_IF_MAP_KEY_MISSING(MAP, KEY) \
	if(MAP.find(KEY) == MAP.end())throw TypeException();

	JSObject::JSObject() : m_obj(NULL)
	{
	}

	JSObject::~JSObject(void)
	{
	}

	void JSObject::initialize(PhysObject::Id obj)
	{
		m_obj = obj;
	}
	void JSObject::invalidate()
	{
		m_obj = 0;
	}

	PhysWorld* JSObject::getWorld() const
	{
		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));

		return static_cast<PhysWorld*>(external->Value());
	}

	PhysObject* JSObject::getObject() const // Return null when we're invalid
	{
		PhysWorld* world = getWorld();
		return world->getObject(m_obj);
	}

	void JSObject::push(vector<double> force) const
	{
		if(force.size() != 3)
			return;

		PhysObject * c = getObject();
		if(c)
		{
			c -> push(btVector3(
				force[0],
				force[1],
				force[2]
			));
		}
	}
	void JSObject::kill()
	{
		PhysObject * c = getObject();
		if(c)
		{
			c->kill();
			invalidate();
		}
	}

	v8::Handle<v8::Object> JSObject::SetupClass(v8::Handle<v8::Object> dest)
	{
		typedef v8::juice::cw::ClassWrap<JSObject> CW;
		typedef v8::juice::convert::MemFuncInvocationCallbackCreator<JSObject> ICM;

		CW & b (CW::Instance());

		b.Set("push", ICM::M1::Invocable<void, vector<double>, &JSObject::push>);
		b.Set("kill", ICM::M0::Invocable<void, &JSObject::kill>);

		b.Seal();
		b.AddClassTo(dest);
		return b.CtorTemplate()->GetFunction();
	}

}

#define CLASSWRAP_BOUND_TYPE Sarona::JSObject
#include <v8/juice/ClassWrap_JuiceBind.h>
