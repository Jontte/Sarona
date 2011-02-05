#include "StdAfx.h"
#include "JSRotation.h"

namespace Sarona
{
	JSRotation::JSRotation(const v8::Handle<v8::Object>& arg)
	{
		x=y=z=0;
		w=1;
		// Arg is either empty or has four scalars
		if(
			arg->Has(0) &&
			arg->Has(1) &&
			arg->Has(2) &&
			arg->Has(3)
			)
		{
			x = arg->Get(0)->ToNumber()->Value();
			y = arg->Get(1)->ToNumber()->Value();
			z = arg->Get(2)->ToNumber()->Value();
			w = arg->Get(3)->ToNumber()->Value();
		}
	}

	void JSRotation::normalize()
	{
		btQuaternion temp(x,y,z,w);
		temp.normalize();
		x = temp.getX();
		y = temp.getY();
		z = temp.getZ();
		w = temp.getW();
	}

	v8::Handle<v8::Object> JSRotation::SetupClass(v8::Handle<v8::Object> dest)
	{
		typedef v8::juice::cw::ClassWrap<JSRotation> CW;
		typedef v8::juice::convert::MemFuncInvocationCallbackCreator<JSRotation> ICM;

		CW & b (CW::Instance());

		b.Set("push", ICM::M0::Invocable<void, &JSRotation::normalize>);
		b.BindMemVar<double, &JSRotation::x>("x");
		b.BindMemVar<double, &JSRotation::y>("y");
		b.BindMemVar<double, &JSRotation::z>("z");
		b.BindMemVar<double, &JSRotation::w>("w");

		b.Seal();
		b.AddClassTo(dest);
		return b.CtorTemplate()->GetFunction();
	}
}


#define CLASSWRAP_BOUND_TYPE Sarona::JSRotation
#include <v8/juice/ClassWrap_JuiceBind.h>

