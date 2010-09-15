#pragma once
#ifndef JSCONVERT_H_
#define JSCONVERT_H_
#include "StdAfx.h"
#include "JSVector.h"
#include "JSRotation.h"
#include <cproxyv8-class.h>

/*
	A couple of utilies to help with conversion from v8::Arguments to proper c++ types
*/

typedef v8::Handle<v8::Value> JSVal_t;
struct TypeException : public std::exception {};

// Single variable conversion

void convert(const JSVal_t& val, JSVal_t& out);
void convert(const JSVal_t& val, string& out);
void convert(const JSVal_t& val, btVector3& out);
void convert(const JSVal_t& val, btQuaternion& out);

// Numeric types
template <class T>
void convert(const JSVal_t& val, T& out, typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = 0)
{
//	if(!val->IsNumber())throw TypeException();
	if(val.IsEmpty())throw TypeException();
	v8::Handle<v8::Number> flt(val->ToNumber());
	out = (T)flt->NumberValue();
}

// generic array
template <class T>
void convert(const JSVal_t& val, vector<T>& out)
{
	if(val.IsEmpty())throw TypeException();
	out.clear();
	v8::Handle<v8::Object> obj = val->ToObject();
	for(unsigned i = 0; i < 0xFFFFFFFF; i++)
	{
		if(!obj->Has(i))break;
		T temporary;
		convert(obj->Get(i), temporary);
		out.push_back(temporary);
	}
}
// generic map
template <class Key, class Value>
void convert(const JSVal_t& val, std::map<Key, Value>& out)
{
	if(val.IsEmpty())throw TypeException();
	out.clear();
	v8::Handle<v8::Object> obj = val->ToObject();

	v8::Local<v8::Array> names = obj->GetPropertyNames();

	for(unsigned i = 0 ; i < names->Length(); i++)
	{
		Value temp1;
		Key temp2;

		convert(obj->Get(names->Get(i))	,	temp1);
		convert(names->Get(i)			,	temp2);

		out[temp2] = temp1;
	}
}

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

// Similar to extract
// Grabs and converts a value from a map
template <class T, class U>
void get(const T& container, const typename T::key_type & key, U& ret)
{
	typename T::const_iterator iter = container.find(key);
	if(iter == container.end())throw TypeException();
	convert(iter->second, ret);
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
