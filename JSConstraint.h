#pragma once
#ifndef JSCONSTRAINT_H_
#define JSCONSTRAINT_H_
#include "StdAfx.h"
#include "PhysWorld.h"

namespace Sarona
{
	class JSConstraint
	{
	friend class PhysWorld;
		PhysWorld::ConstraintID m_constraint;
		PhysWorld* getWorld() const;
		btTypedConstraint* getConstraint();
	public:
		JSConstraint();
		void setMotor(bool enable, double target, double max_impulse);
		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
	};

}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::JSConstraint, "Constraint");

#define CLASSWRAP_BOUND_TYPE Sarona::JSConstraint
#include <v8/juice/ClassWrap-JSToNative.h>

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::JSConstraint> : Factory_CtorForwarder<Sarona::JSConstraint,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::JSConstraint>
		>
	>
	{};
}}}

#endif
