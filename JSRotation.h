#pragma once
#ifndef JSROTATION_H_
#define JSROTATION_H_
#include "StdAfx.h"

namespace Sarona
{
	class JSRotation
	{
	public:
		double x,y,z,w;
		JSRotation(const v8::Handle<v8::Object>& arg = v8::Handle<v8::Object>());

		void normalize();
	};
}

#endif
