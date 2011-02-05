#pragma once
#ifndef JSOBJECT_H_
#define JSOBJECT_H_
#include "StdAfx.h"
#include "JSConvert.h"
#include "PhysObject.h"
#include <v8/juice/WeakJSClassCreator.h>

namespace Sarona
{
	class JSObject
	{
	private:

		// Weak ref to the physical object
		PhysObject::Id m_obj;

		PhysWorld* getWorld() const;

	public:
		PhysObject* getObject() const; // Return null when we're invalid

		void initialize(PhysObject::Id obj = 0);
		void invalidate();

		JSObject();
		~JSObject();

		void push(vector<double> force) const;
		void kill();

		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
	};
}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::JSObject, "ObjectHandle");

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::JSObject> : Factory_CtorForwarder<Sarona::JSObject,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::JSObject>
		>
	>
	{};
}}}/*
namespace v8 { namespace juice { namespace convert {
template <> struct JSToNative<Sarona::JSObject>
{
	typedef Sarona::JSObject* ResultType;
	Sarona::JSObject* operator()( v8::Handle<v8::Value> const & h ) const
	{
		Sarona::JSObject* obj = ::v8::juice::WeakJSClassCreator<Sarona::JSObject>::GetNative( h );
		Sarona::PhysObject* phys = obj->getObject();
		if(phys)
			return phys->getNetworkId();
		return -1;
	}
};
} } }*/ // namespaces
#endif
