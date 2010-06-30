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
}

