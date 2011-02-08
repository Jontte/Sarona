#pragma once
#ifndef JSOBJECT_H_
#define JSOBJECT_H_
#include "StdAfx.h"
#include "JSConvert.h"
#include "PhysObject.h"

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

		double getFriction();
		void   setFriction(double);
		double getRestitution();
		void   setRestitution(double);

		JSObject();
		~JSObject();

		void push(double x, double y, double z) const;
		void push(btVector3 force) const;
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
}}}

#define CLASSWRAP_BOUND_TYPE Sarona::JSObject
#include <v8/juice/ClassWrap_TwoWay.h>

//JUICE_CLASSWRAP_JSTONATIVE(Sarona::JSObject)

#endif
