#include "StdAfx.h"
#include "JSObject.h"
#include "JSConvert.h"

#include "PhysWorld.h"

namespace Sarona
{
#define THROW_IF_MAP_KEY_MISSING(MAP, KEY) \
	if(MAP.find(KEY) == MAP.end())throw TypeException();

	JSObject::JSObject(const v8::Handle<v8::Object>& args)
		: m_obj(0)
	{
		std::string mesh;
		std::string body;
		std::string texture;
		btScalar mass(0);
		btVector3 position(0,0,0);
		btQuaternion rotation(btVector3(0,1,0), 0.0);

		if(!args->Has(0))
			throw std::out_of_range("Too few arguments!");
		if(!args->Get(0)->IsObject())
			throw TypeException();

		v8::Handle<v8::Object> config = args->Get(0)->ToObject();

		std::map<std::string, v8::Handle<v8::Value>> params;

		convert(args->Get(0), params);

		// Required parameters:
		try
		{
			get(params, "body", body);
			get(params, "mesh", mesh);
		} catch(...){}
		//MANAGE_THROWS("Object","Ctor");

		// Optional:
		try	{ get(params, "position", position); } catch(...){}
		try	{ get(params, "rotation", rotation); } catch(...){}

		// Success! Create the object

		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));

		PhysWorld* world = static_cast<PhysWorld*>(external->Value());

		PhysObject * object = world->CreateObject(position, rotation);
		m_obj = object->getNetworkId();

		object -> setBody(body);
		object -> setMesh(mesh);

		// Optional:
		// Position:
		try
		{
			get(params, "texture", texture);
			object->setTexture(texture);
		} catch(...){}
		// Mass:
		try
		{
			get(params, "mass", mass);
			object->setMass(mass);
		} catch(...){}
		// MeshScaling:
		try
		{
			float meshScale;
			get(params, "meshScale", meshScale);
			object->setMeshScale(meshScale);
		} catch(...){}
		// BodyScaling:
		try
		{
			float bodyScale = 1;
			get(params, "bodyScale", bodyScale);
			object->setBodyScale(bodyScale);
		} catch(...){}
	}

	JSObject::~JSObject(void)
	{
	}

	void JSObject::invalidate()
	{
		m_obj = 0;
	}

	PhysWorld* JSObject::getWorld()
	{
		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));

		return static_cast<PhysWorld*>(external->Value());
	}
	
	PhysObject*  JSObject::getObject() // Return null when we're invalid
	{
		PhysWorld* world = getWorld();
		return world->getObject(m_obj);
	}
	
	v8::Handle<v8::Value> JSObject::push(const v8::Arguments& arg)
	{
		try
		{
			float x,y,z;
			extract(arg, x, y, z);

			PhysObject * c = getObject();
			if(c)
			{
				c -> push(btVector3(x,y,z));
			}

			return v8::Handle<v8::Value>();
		}
		MANAGE_THROWS("Object","push");
	}
	v8::Handle<v8::Value> JSObject::kill(const v8::Arguments& args)
	{
		PhysObject * c = getObject();
		if(c)
		{
			c->kill();
			// Invalidate ourselves..
			m_obj = 0;
		}
		return v8::Handle<v8::Value>();
	}

}
