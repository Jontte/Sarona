// Copyright 2008 the CProxyV8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cassert>

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

#include "cproxyv8.h"


#include <vector>

/* this is used to convert a v8::Arguments to a vector of v8 handles to v8 values */
void args2vector(const v8::Arguments& in, v8::Handle<v8::Object>& out)
{
	// Arguments.operator[] does not guarantee contiguous space
	// so lets just copy the stuff the old way

//	v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();

//		v8::Local<v8::External> external = v8::Local<v8::External>::Cast(in.Data());//GetInternalField(0));

//		PhysWorld* world = static_cast<PhysWorld*>(external->Value());

/*
	v8::Local<v8::ObjectTemplate> tpl = v8::ObjectTemplate::New();
	tpl->SetInternalFieldCount(1);
*/
	out = v8::Persistent<v8::Object>::New(v8::Object::New()); //tpl->NewInstance();

//	out->SetInternalField(0, external);

	for(int i = 0 ; i < in.Length(); i++)
	{
		out->Set(i, in[i]);
	}
}

void Accessor_SetProperty(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  v8::Local<v8::External> external = v8::Local<v8::External>::Cast(info.Data());
  AccessorProperty* callBack = static_cast<AccessorProperty*>(external->Value());
  callBack->AccessorSetProperty(property, value, info);
}

v8::Handle<v8::Value> Accessor_GetProperty(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
  v8::Local<v8::External> external = v8::Local<v8::External>::Cast(info.Data());
  AccessorProperty* callBack = static_cast<AccessorProperty*>(external->Value());
  return callBack->AccessorGetProperty(property,  info);
}


v8::Handle<v8::Value> Accessor_Function(const v8::Arguments& args)
{
  v8::Local<v8::External> external = v8::Local<v8::External>::Cast(args.Data());
  AccessorFunction* callBack = static_cast<AccessorFunction*>(external->Value());
  return callBack->call(args);
}

