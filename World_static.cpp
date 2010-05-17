#include "StdAfx.h"
#include "World.h"

/* 
	This file contains all of the static JS callback functions defined for 'World'
*/

namespace Sarona
{

	// Static functions

	v8::Handle<v8::Value> World::JSPrint(const v8::Arguments& args)
	{
		bool first = true;
		for (int i = 0; i < args.Length(); i++)
		{
			v8::HandleScope handle_scope;
			if (first)
			{
				first = false;
			}
			else
			{
				std::cout << " ";
			}
			v8::String::Utf8Value str(args[i]);
			
			std::cout << *str;
		}
		std::cout << std::endl;

		return v8::Undefined();
	}
	
	v8::Handle<v8::Value> World::JSSceneCreate(const v8::Arguments& args)
	{
		World* world = static_cast<World*>(v8::Handle<v8::External>::Cast(args.Holder()->GetInternalField(0))->Value());
		return world->SceneCreate(args);
	}

	v8::Handle<v8::Value> World::JSSceneGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		World* world = static_cast<World*>(v8::Handle<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
		return world->SceneGetter(property, info);
	}
	void World::JSSceneSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		World* world = static_cast<World*>(v8::Handle<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
		return world->SceneSetter(property,value,info);	
	}


}

