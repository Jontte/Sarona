#pragma once
#ifndef JSROTATION_H_
#define JSROTATION_H_
#include "StdAfx.h"
#include "JSConvert.h"

namespace Sarona
{
	class JSRotation
	{
	public:
		double x,y,z,w;
		JSRotation(const v8::Handle<v8::Object>& arg = v8::Handle<v8::Object>());

		void normalize();
		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
	};
}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::JSRotation, "Rotation");

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::JSRotation> : Factory_CtorForwarder<Sarona::JSRotation,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::JSRotation>
		>
	>
	{};
}}}
namespace v8 { namespace juice { namespace convert {
template <> struct JSToNative<btQuaternion>
{
	typedef btQuaternion ResultType;
	btQuaternion operator()( v8::Handle<v8::Value> const & h ) const
	{
		btQuaternion ret;
		if(!h->IsObject())
			throw TypeException();
		v8::Handle<v8::Object> obj = h->ToObject();
		if(!obj->Has(0) || !obj->Has(1) || !obj->Has(2) || !obj->Has(3))
			throw TypeException();

		ret.setValue(
			obj->Get(0)->NumberValue(),
			obj->Get(1)->NumberValue(),
			obj->Get(2)->NumberValue(),
			obj->Get(3)->NumberValue()
		);
		return ret;
	}
};
} } } // namespaces
#endif
