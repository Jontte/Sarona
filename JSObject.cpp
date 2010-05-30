#include "StdAfx.h"
#include "JSObject.h"
#include "JSConvert.h"

#include "PhysWorld.h"

namespace Sarona
{

	JSObject::JSObject(const v8::Handle<v8::Object>& args)
		: m_obj(NULL)
	{
		std::string mesh;
		std::string body;
		std::string texture;
		btVector3 position(0,0,0);

		if(!args->Has(0))
			throw std::out_of_range("Too few arguments!");
		if(!args->Get(0)->IsObject())
			throw TypeException();

		v8::Handle<v8::Object> config = args->Get(0)->ToObject();

		// Required parameters:
		//try
		//{

			dig(config, "body", body);
			dig(config, "mesh", mesh);
		//}
		
		//MANAGE_THROWS("Object","Ctor");

		// Optional:
		try
		{
			dig(config, "position", position);
		} catch(...){}


		// Success! Create the object

		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));

		PhysWorld* world = static_cast<PhysWorld*>(external->Value());

		m_obj = world->CreateObject(position);
		m_obj -> setBody(body);
		m_obj -> setMesh(mesh);

		// Optional:
		try
		{
			dig(config, "texture", texture);
			m_obj->setTexture(texture);
		} catch(...){}
	}

	JSObject::~JSObject(void)
	{
	}

	void JSObject::invalidate()
	{
		m_obj = NULL;
	}
	
	v8::Handle<v8::Value> JSObject::push(const v8::Arguments& arg)
	{
		try
		{
			double x,y,z;
			extract(arg, x, y, z);


			return v8::Handle<v8::Value>();
		}
		MANAGE_THROWS("Object","push");
	}

}
