#include "StdAfx.h"
#include "JSConstraint.h"

namespace Sarona
{
	JSConstraint::JSConstraint() : m_constraint(0)
	{

	}

	PhysWorld* JSConstraint::getWorld() const
	{
		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(
			v8::Handle<v8::Object>::Cast(v8::Context::GetCurrent()->Global()->GetPrototype())
				->GetInternalField(0));

		return static_cast<PhysWorld*>(external->Value());
	}

	btTypedConstraint* JSConstraint::getConstraint()
	{
		PhysWorld* world = getWorld();
		if(!world)
			return NULL;

		return world->GetConstraint(m_constraint);
	}

	/*
		For hinge type constraints, target parameter in setMotor() represents target angular velocity
	*/
	void JSConstraint::setMotor(bool enable, double target, double max_impulse)
	{
		btTypedConstraint * c = getConstraint();
		if(!c)return;

		btTypedConstraintType type = c->getConstraintType();
		switch(type)
		{
			case HINGE_CONSTRAINT_TYPE:
			{
				// This ought to be safe now.
				btHingeConstraint * hinge = static_cast<btHingeConstraint*>(c);
				hinge->enableAngularMotor(enable, target, max_impulse);
			}
				break;
			default:
				break;
		}
	}

	v8::Handle<v8::Object> JSConstraint::SetupClass(v8::Handle<v8::Object> dest)
	{
		typedef v8::juice::cw::ClassWrap<JSConstraint> CW;
		typedef v8::juice::convert::MemFuncInvocationCallbackCreator<JSConstraint> ICM;

		CW & b (CW::Instance());

		b.Set("setMotor", ICM::M3::Invocable<void, bool, double, double, &JSConstraint::setMotor>);

		b.Seal();
		b.AddClassTo(dest);
		return b.CtorTemplate()->GetFunction();
	}
}


