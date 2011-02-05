#include "StdAfx.h"
#include "JSVector.h"

namespace Sarona
{
	JSVector::JSVector(const v8::Handle<v8::Object>& arg)
	{
		// Arg is either empty or has three scalars
		if(
			arg->Has(0) &&
			arg->Has(1) &&
			arg->Has(2)
			)
		{
			x = arg->Get(0)->ToNumber()->Value();
			y = arg->Get(1)->ToNumber()->Value();
			z = arg->Get(2)->ToNumber()->Value();
		}
	}
	JSVector::JSVector(double x_, double y_, double z_)
		: x(x_), y(y_), z(z_)
	{}


	v8::Handle<v8::Object> JSVector::SetupClass(v8::Handle<v8::Object> dest)
	{
		typedef v8::juice::cw::ClassWrap<JSVector> CW;
		typedef v8::juice::convert::MemFuncInvocationCallbackCreator<JSVector> ICM;

		CW & b (CW::Instance());

		b.BindMemVar<double, &JSVector::x>("x");
		b.BindMemVar<double, &JSVector::y>("y");
		b.BindMemVar<double, &JSVector::z>("z");;

		b.Seal();
		b.AddClassTo(dest);
		return b.CtorTemplate()->GetFunction();
	}
}


