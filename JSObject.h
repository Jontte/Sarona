#pragma once
#include "StdAfx.h"
#include "PhysObject.h"

namespace Sarona
{
	class JSObject
	{
	private:
		
		// Weak ref to the physical object
		PhysObject* m_obj; 

	public:

		void invalidate();

		JSObject(const v8::Handle<v8::Object>& args = v8::Handle<v8::Object>() );
		~JSObject(void);

		v8::Handle<v8::Value> push(const v8::Arguments&);
	};
}
