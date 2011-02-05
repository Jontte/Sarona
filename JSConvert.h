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

		template <class T>
		mapper operator()(string fieldname, T& fieldvar, int flags)
		{
			v8::Handle<v8::String> key = v8::String::New(fieldname.c_str());
			if(src->Has(key))
			{
				fieldvar = ::v8::juice::convert::CastFromJS<T>(src->Get(key));
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
	};

	mapper emap(jsobj& obj);
}

/*
	This code is used to simplify the catch(...) part around your JSObject member functions
*/

#define MANAGE_THROWS(CLASSNAME, METHODNAME)\
	catch(TypeException&)\
	{\
		return ThrowException(v8::String::New("Type conversion error in "CLASSNAME"::"METHODNAME));\
	}\
	catch(std::out_of_range&)\
	{\
		return ThrowException(v8::String::New("Invalid number of arguments to "CLASSNAME"::"METHODNAME));\
	}\
	catch(...)\
	{\
		return ThrowException(v8::String::New("Unknown exception in "CLASSNAME"::"METHODNAME));\
	}


#endif
