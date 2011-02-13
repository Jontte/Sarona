#include "StdAfx.h"
#include "JSRotation.h"
#include "JSConvert.h"

namespace extractor
{
	mapper emap(jsobj& obj)
	{
		mapper m;
		m.src = obj;
		m.last_operation_result = false;
		return m;
	};
	mapper emap(v8::Handle<v8::Value>& val)
	{
		if(!val->IsObject())
			throw TypeException();
		v8::Handle<v8::Object> obj = val->ToObject();
		return emap(obj);
	};
}
/*
// Implementation for the static JS-type conversion funcs

// Passthru
void convert(const JSVal_t& val, JSVal_t& out)
{
	if(val.IsEmpty())throw TypeException();
	out = val;
}

// string
void convert(const JSVal_t& val, string& out)
{
	if(val.IsEmpty())throw TypeException();
	v8::Handle<v8::String> str(val->ToString());
	out.resize(str->Utf8Length());
	str->WriteUtf8(&out[0], out.size());
}
// vector
void convert(const JSVal_t& val, btVector3& out)
{
	if(val.IsEmpty())throw TypeException();
	if(!val->IsObject())throw TypeException();

	v8::Handle<v8::Object> object = val->ToObject();

	// Check if the object is a native JS Array of the form [x,y,z]
	if(
		object->Has(0) &&
		object->Has(1) &&
		object->Has(2)
		)
	{
		// Array type
		convert<btScalar>(val->ToObject()->Get(0), out[0]);
		convert<btScalar>(val->ToObject()->Get(1), out[1]);
		convert<btScalar>(val->ToObject()->Get(2), out[2]);
		return;
	}
	else
	{
		// Native type
		CProxyV8::InstaceHandleBase* base = static_cast<CProxyV8::InstaceHandleBase*>(v8::Handle<v8::External>::Cast(object->GetInternalField(0))->Value());

		if(base->Is<Sarona::JSVector>())
		{
			Sarona::JSVector * vec = base->GetObject<Sarona::JSVector>();
			out[0] = (btScalar)vec->x;
			out[1] = (btScalar)vec->y;
			out[2] = (btScalar)vec->z;
			return;
		}
	}
	// Something went wrong
	throw TypeException();
}
// quaternion/rotation
void convert(const JSVal_t& val, btQuaternion& out)
{
	if(val.IsEmpty())throw TypeException();
	if(!val->IsObject())throw TypeException();

	v8::Handle<v8::Object> object = val->ToObject();

	// Check if the object is a native JS Array of the form [x,y,z,w]
	if(
		object->Has(0) &&
		object->Has(1) &&
		object->Has(2) &&
		object->Has(3)
		)
	{
		// Array type
		convert<btScalar>(val->ToObject()->Get(0), out[0]);
		convert<btScalar>(val->ToObject()->Get(1), out[1]);
		convert<btScalar>(val->ToObject()->Get(2), out[2]);
		convert<btScalar>(val->ToObject()->Get(3), out[3]);
		return;
	}
	else
	{
		// Native type
		CProxyV8::InstaceHandleBase* base = static_cast<CProxyV8::InstaceHandleBase*>(v8::Handle<v8::External>::Cast(object->GetInternalField(0))->Value());

		if(base->Is<Sarona::JSRotation>())
		{
			Sarona::JSRotation * vec = base->GetObject<Sarona::JSRotation>();
			out[0] = (btScalar)vec->x;
			out[1] = (btScalar)vec->y;
			out[2] = (btScalar)vec->z;
			out[3] = (btScalar)vec->w;
			vec->normalize();
			return;
		}
	}
	// Something went wrong
	throw TypeException();
}

*/
