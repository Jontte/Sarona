#pragma once
#include "StdAfx.h"

/*
	A couple of utilies to help with conversion from v8::Arguments to proper c++ types
*/

typedef v8::Handle<v8::Value> JSVal_t;
struct TypeException : public std::exception {};

// Single variable conversion

template <class T>
T convert(const JSVal_t& val);

// Numeric types
template <class T>
T convert<typename boost::enable_if<boost::is_arithmetic<T>, T>::type>(const JSVal_t& val)
{ 
	if(!val->IsNumber())throw TypeException();
	v8::Handle<v8::Number> flt(val->ToNumber());
	return (T)flt->NumberValue();
}

// string
template <>
string convert<string>(const JSVal_t& val)
{
	if(!val->IsString())throw TypeException();
	v8::Handle<v8::String> str(val->ToString());

	string ret;
	ret.resize(str->Utf8Length());
	str->WriteUtf8(&ret[0], ret.size());
	return ret;
}
// vector
template <>
btVector3 convert<btVector3>(const JSVal_t& val)
{
	if(!val->IsObject())throw TypeException();
	btVector3 ret;
	ret[0] = convert<btScalar>(val->ToObject()->Get(0));
	ret[1] = convert<btScalar>(val->ToObject()->Get(1));
	ret[2] = convert<btScalar>(val->ToObject()->Get(2));
	return ret;
}

template <class T1> void extract(const v8::Arguments& arg, T1& t1) {
	if(arg.Length() < 1)throw std::out_of_range("Out of range");
	t1 = convert<T1>(arg[0]);
}
template <class T1, class T2> void extract(const v8::Arguments& arg, T1& t1, T2& t2) {
	if(arg.Length() < 2)throw std::out_of_range("Out of range");
	t1 = convert<T1>(arg[0]);
	t2 = convert<T2>(arg[1]);
}
template <class T1, class T2, class T3> void extract(const v8::Arguments& arg, T1& t1, T2& t2, T3& t3) {
	if(arg.Length() < 3)throw std::out_of_range("Out of range");
	t1 = convert<T1>(arg[0]);
	t2 = convert<T2>(arg[1]);
	t3 = convert<T3>(arg[2]);
}
template <class T1, class T2, class T3, class T4> void extract(const v8::Arguments& arg, T1& t1, T2& t2, T3& t3, T4& t4) {
	if(arg.Length() < 4)throw std::out_of_range("Out of range");
	t1 = convert<T1>(arg[0]);
	t2 = convert<T2>(arg[1]);
	t3 = convert<T3>(arg[2]);
	t4 = convert<T4>(arg[2]);
}
// Similar to extract..
template <class T>
void dig(v8::Handle<v8::Value> val, std::string name, T& t)
{
	if(!val->IsObject())throw TypeException();
	t = convert<T>(val->ToObject()->Get(v8::String::New(name.c_str())));
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




