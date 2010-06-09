#pragma once
#include "StdAfx.h"

namespace Sarona
{

	class JSVector
	{
	public:

		double x,y,z;

		JSVector(const v8::Handle<v8::Object>& arg = v8::Handle<v8::Object>());
	};

}

