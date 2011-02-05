#include "StdAfx.h"
#include "PhysWorld.h"

/*
	This file contains all of the static JS callback functions defined for 'World'
*/

namespace Sarona
{

	// Static functions

	v8::Handle<v8::Value> PhysWorld::JSPrint(const v8::Arguments& args)
	{
		for (int i = 0; i < args.Length(); i++)
		{
			v8::HandleScope handle_scope;

			if (i > 0)
				std::cout << " ";

			v8::String::Utf8Value str(args[i]);
			std::cout << *str;
		}
		std::cout << std::endl;

		return v8::Undefined();
	}


#define IMPL_FUNC(OBJECT,WRAPFUNC,REALFUNC)\
	v8::Handle<v8::Value> OBJECT::WRAPFUNC(const v8::Arguments& args)\
	{\
		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(\
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())\
				->GetInternalField(0));\
		OBJECT * handle = static_cast<OBJECT*>(external->Value());\
		return handle->REALFUNC(args);\
	}


IMPL_FUNC(PhysWorld,	JSSetTimeout,				SetTimeout);
IMPL_FUNC(PhysWorld,	JSCreateObject,				CreateObject);

	/*
	v8::Handle<v8::Value> PhysWorld::JSSceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		PhysWorld* world = static_cast<PhysWorld*>(v8::Handle<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
		return world->SceneGetter(property, info);
	}
	void PhysWorld::JSSceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PhysWorld* world = static_cast<PhysWorld*>(v8::Handle<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
		return world->SceneSetter(property,value,info);
	}*/


}

