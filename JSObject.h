#pragma once
#include "StdAfx.h"
#include "PhysObject.h"

namespace Sarona
{
	class JSObject
	{
	private:
		
		// Weak ref to the physical object
		PhysObject::Id m_obj; 

		PhysWorld* getWorld();

	public:
		PhysObject* getObject(); // Return null when we're invalid

		void invalidate();

		JSObject(const v8::Handle<v8::Object>& args = v8::Handle<v8::Object>() );
		~JSObject(void);
		
		v8::Handle<v8::Value> push(const v8::Arguments&);
		v8::Handle<v8::Value> kill(const v8::Arguments&);
	};
}
