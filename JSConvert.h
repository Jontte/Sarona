#pragma once
#ifndef JSCONVERT_H_
#define JSCONVERT_H_
#include "StdAfx.h"

/*
	A couple of utilies to help with conversion from v8::Arguments to proper c++ types
*/

typedef v8::Handle<v8::Value> JSVal_t;
struct TypeException : public std::exception {};


template <class T1> void extract(const v8::Arguments& arg, T1& t1) {
	if(arg.Length() < 1)throw std::out_of_range("Out of range");
	convert(arg[0], t1);
}
template <class T1, class T2> void extract(const v8::Arguments& arg, T1& t1, T2& t2) {
	if(arg.Length() < 2)throw std::out_of_range("Out of range");
	convert(arg[0], t1);
	convert(arg[1], t2);
}
template <class T1, class T2, class T3> void extract(const v8::Arguments& arg, T1& t1, T2& t2, T3& t3) {
	if(arg.Length() < 3)throw std::out_of_range("Out of range");
	convert(arg[0], t1);
	convert(arg[1], t2);
	convert(arg[2], t3);
}
template <class T1, class T2, class T3, class T4> void extract(const v8::Arguments& arg, T1& t1, T2& t2, T3& t3, T4& t4) {
	if(arg.Length() < 4)throw std::out_of_range("Out of range");
	convert(arg[0], t1);
	convert(arg[1], t2);
	convert(arg[2], t3);
	convert(arg[2], t4);
}

namespace extractor
{
	typedef v8::Handle<v8::Object> jsobj;

	enum {
		REQUIRED = 1
	};

	struct mapper
	{
		jsobj src;
		bool last_operation_result; // 1 = success, 0 = failure

		template <class T>
		mapper operator()(string fieldname, T& fieldvar, int flags = 0)
		{
			last_operation_result = false;

			v8::Handle<v8::String> key = v8::String::New(fieldname.c_str());
			if(src->Has(key))
			{
				try
				{
					fieldvar = ::v8::juice::convert::CastFromJS<T>(src->Get(key));
					last_operation_result = true;
				}
				catch(...)
				{
					// Propagate error only if this field was marked as required
					if(flags & REQUIRED)
						throw;
				}
			}
			else
			{
				if(flags & REQUIRED)
				{
					throw TypeException();
				}
			}
			return *this; // copy of self
		}
		mapper operator()(string fieldname, v8::Handle<v8::Object> & fieldvar, int flags = 0)
		{
			v8::Handle<v8::Value> val;
			(*this)(fieldname, val, flags);
			if(!val->IsObject())
				throw TypeException();
			fieldvar = val->ToObject();
			return *this;
		}
		operator bool()
		{
			return last_operation_result;
		}
	};

	mapper emap(jsobj& obj);
	mapper emap(v8::Handle<v8::Value>& obj);
}

#endif
