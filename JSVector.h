#pragma once
#ifndef JSVECTOR_H_
#define JSVECTOR_H_
#include "StdAfx.h"
#include "JSConvert.h"

namespace Sarona
{
	class JSVector
	{
	public:
		double x,y,z;
		JSVector(const v8::Handle<v8::Object>& arg = v8::Handle<v8::Object>());

		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
	};

}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::JSVector, "Vector");

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::JSVector> : Factory_CtorForwarder<Sarona::JSVector,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::JSVector>
		>
	>
	{};
}}}
namespace v8 { namespace juice { namespace convert {
template <> struct JSToNative<btVector3>
{
	typedef btVector3 ResultType;
	btVector3 operator()( v8::Handle<v8::Value> const & h ) const
	{
		btVector3 ret;
		if(!h->IsObject())
			throw TypeException();
		v8::Handle<v8::Object> obj = h->ToObject();
		if(!obj->Has(0) || !obj->Has(1) || !obj->Has(2))
			throw TypeException();

		ret.setValue(
			obj->Get(0)->NumberValue(),
			obj->Get(1)->NumberValue(),
			obj->Get(2)->NumberValue()
		);
		return ret;
	}
};

} } } // namespaces
#endif
