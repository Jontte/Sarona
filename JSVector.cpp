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
}

