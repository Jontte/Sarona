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
		JSVector(double x, double y, double z);

		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
	};

}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::JSVector, "Vector");

#define CLASSWRAP_BOUND_TYPE Sarona::JSVector
#include <v8/juice/ClassWrap-JSToNative.h>

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::JSVector> : Factory_CtorForwarder<Sarona::JSVector,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::JSVector>,
			convert::CtorForwarder3<Sarona::JSVector, double, double, double>
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
		if(!h->IsObject())
			throw TypeException();
		v8::Handle<v8::Object> obj = h->ToObject();

		// check for plain array [x,y,z]
		if(obj->Has(0) && obj->Has(1) && obj->Has(2))
		{
			return btVector3(
				obj->Get(0)->NumberValue(),
				obj->Get(1)->NumberValue(),
				obj->Get(2)->NumberValue()
			);
		}

		// check for a JSVector object (new Vector called in js)
		Sarona::JSVector * vec = v8::juice::convert::CastFromJS<Sarona::JSVector>(obj);
		if(vec)
		{
			return btVector3(vec->x, vec->y, vec->z);
		}

		// No match.
		throw TypeException();
	}
};

} } } // namespaces

#endif
