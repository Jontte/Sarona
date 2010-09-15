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

#ifndef CPROXYV8_H_
#define CPROXYV8_H_

#include <v8.h>
#include <string>
#include <typeinfo>
#include <cstdlib>
#include <string.h> //for strcmp...

template <class T>
class InstaceHandle;

template <class T>
class THandlePrimitive;

template<class T>
class ProxyClass;

class Accessor
{
};
/**
 * AccessorProperty allow the program to make the abstraction betweem the real
 * cppObj->property and the generic callback from JS
 *
 * The idea here is to take advantage of the CTemplates and the pure virtual
 * methods, to avoid the need to specify the CTemplate type in the access to the
 * cppObj->property
 *
 * This kind of integration requires args.Data() to be an implementation
 * of AccessorProperty and args.Holder() the instance of the specific object.
 *
 * This are the current supported properties:
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      TPrimitive cppObj->Method();   //see FuncRetPrimitive
 *      TClass cppObj->Method();       //see FuncRetObj
 *      v8::Handle<v8::Value> Method(const v8::Arguments& args);  //FuncV8
 *    ...
 * \endcode
 *
 */
class AccessorProperty : public Accessor
{
public:

  /** Changet the value of the property in the Cpp generated program */
	virtual void AccessorSetProperty(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) = 0;

  /** returns the value of the property to JS enviroment */
	virtual v8::Handle<v8::Value> AccessorGetProperty(v8::Local<v8::String> property, const v8::AccessorInfo& info) = 0;
};

/**
 * JS callback
 *
 * This function obtains the generic AccessorProperty from args,
 * and set the value of cppObj->property with the value
 */
void Accessor_SetProperty(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

/**
 * JS callback
 *
 * This function obtains the generic AccessorProperty from args,
 * and return the value of cppObj->property
 */
v8::Handle<v8::Value> Accessor_GetProperty(v8::Local<v8::String> property, const v8::AccessorInfo& info);

/**
 * AccessorFunction allow the program to make the abstraction betweem the real
 * cppObj->Method() and the generic callback from JS
 *
 * The idea here is to take advantage of the CTemplates and the pure virtual
 * methods, to avoid the need to specify the CTemplate type in the call to the
 * cppObj->Method()
 *
 * This kind of integration requires args.Data() to be an implementation
 * of AccessorFunction and args.Holder() the instance of the specific object.
 *
 * This are the current supported methods:
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      TPrimitive cppObj->Method();   //see FuncRetPrimitive
 *      TClass cppObj->Method();       //see FuncRetObj
 *      v8::Handle<v8::Value> Method(const v8::Arguments& args);  //FuncV8
 *    ...
 * \endcode
 *
 */
class AccessorFunction : public Accessor
{
public:

  /** Entry point from JS. */
	virtual v8::Handle<v8::Value> call(const v8::Arguments& args) = 0;
};

/**
 * JS callback
 *
 * This function obtains the generic AccessorFunction from args,
 * and call the proxy to call cppObj->method()
 */
v8::Handle<v8::Value> Accessor_Function(const v8::Arguments& args);

namespace internalUtil
{
  /**
   * Utility class to keep track of all accessors in order to be delete when
   * the ProxyClass is disposed.
   */
  class AccessorList
  {
  public:
    AccessorList() : next_(NULL), accessor_(NULL) {}
    AccessorList(Accessor* accessor) : next_(NULL), accessor_(accessor) {}

    inline virtual ~AccessorList();

    inline void push(Accessor * accessor);

  private:
    AccessorList* next_;
    Accessor* accessor_;
  };


  AccessorList::~AccessorList()
  {
    if (accessor_)
      delete accessor_;

    if (next_)
      delete next_;
  }

  void AccessorList::push(Accessor * accessor)
  {
    AccessorList* accessorList = new AccessorList(accessor);
    accessorList = next_;
    next_ = accessorList;
  }
}

namespace CProxyV8
{
    using namespace v8;
/**
 * ObjectProperty is the implementation of AccessorProperty that works
 * as a proxy between properties of type:
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      TClass property;
 *    ...
 * \endcode
 *
 * Where TClass represents the class of objects that already have a
 * CProxyV8::ProxyClass, this will expose the property object to JS enviroment
 *
 *
 * issues: TClass can not be a pointer(*), you will receive a template ambiguous
 *         type cast error if you use it.
 *
 * \code
 *    {
 *    public:
 *      Point  property; //ok
 *      Point* property; //ambiguous type
 *
 * \endcode
 *
 * Be aware that this expose the object to JS and it can be manipulated
 * to change the state (properties values inside the returned object).
 *
 * see: CProxyV8::ProxyClass
 */
template<class T, class TProperty>
class ObjectProperty : public AccessorProperty
{
public:
	typedef TProperty T::*PropertyDir;

	ObjectProperty( PropertyDir propertyDir)
               : propertyDir_(propertyDir) {}

	virtual void AccessorSetProperty( v8::Local<v8::String> property,
                                    v8::Local<v8::Value> value,
                                    const v8::AccessorInfo& info);

	virtual v8::Handle<v8::Value> AccessorGetProperty( v8::Local<v8::String> property,
                                                     const v8::AccessorInfo& info);

//  ProxyClass<T>* getTemplateClass() { return templateClass_; };

private:
	PropertyDir propertyDir_;
};


/**
 * TAccessorFunction is a handy class, it keeps the commom variables for the
 * specific implemtations of AccessorFunction.
 */
template<class T, class TReturn>
class TAccessorFunction : public AccessorFunction
{
public:
  typedef TReturn (T::*Function)();

  TAccessorFunction(Function function)
                  : function_(function) {}

protected:
  Function function_;
};

/**
 * FuncRetObj is the implementation of AccessorFunction that works
 * as a proxy between functions of type:
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      TClass Method();
 *    ...
 * \endcode
 *
 * Where TClass represents the class of objects that already have a
 * CProxyV8::ProxyClass, this will create a new jsObj of the class TClass and
 * returned to the JS enviroment.
 *
 * No arguments allowed
 *
 * issues: TClass can not be a pointer(*) or a references (&), you will
 *         receive a template ambiguous type cast error if you use it.
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      Point  Method(); //ok
 *      Point* Method(); //ambiguous type
 *      Point& Method(); //ambiguous type
 * \endcode
 *
 *         With this limitation there is an overhead that create duplicate objets,
 *         at least 2. If you need to pass complex objects use FuncV8.
 *
 * see: CProxyV8::ProxyClass
 * see: CProxyV8::FuncV8
 */

template<class T, class TClass>
class FuncRetObj : public TAccessorFunction<T, TClass>
{
public:
	typedef TClass (T::*Function)();

	FuncRetObj(Function function)
     :  TAccessorFunction<T, TClass>(function)
  {}

	virtual v8::Handle<v8::Value> call(const v8::Arguments& args);
};

template<class T, class TClass>
v8::Handle<v8::Value> FuncRetObj<T,TClass>::call(const v8::Arguments& args)
{
  T* t = ProxyClass<T>::instance()->UnwrapObject(args.Holder());
//  return Type::FCast((t->*function_)());
return v8::Handle<v8::Value>();
}

/**
 * FuncV8 is the more generic implementation of the AccessorFunction it works
 * almost as the generic V8 callback but with the great advantage that you
 * receive the call inside the object instance, allowing you to access this Cpp
 * implementation more easily.
 *
 * FuncV8 works as a proxy between functions of type:
 *
 * \code
 *    class Example
 *    {
 *    public:
 *      v8::Handle<v8::Value> Method(const v8::Arguments& args);
 *    ...
 * \endcode
 *
 * The following example shows how to use a FuncV8 proxy:
 *
 * \code
 *    class Point
 *    {
 *    public:
 *        ...
 *        v8::Handle<v8::Value> setX(const v8::Arguments& args);
 *        v8::Handle<v8::Value> setY(const v8::Arguments& args);
 *        ...
 *    };
 *    ...
 *    ProxyClass<Point>* bridgePoint = NEW_BRIDGE_CLASS(Point);
 *    bridgePoint->ExposeFunc("setX", &Point::SetX);
 *    bridgePoint->ExposeFunc("setY", &Point::SetY);
 *
 *    ------------ Java Script console:
 *
 *    >x = new Point();
 *    [object]
 *    >x.setX(10);
 *    10
 * \endcode
 *
 */
typedef v8::Handle<v8::Value> V8Value;
template<class T>
class FuncV8 : public TAccessorFunction<T, V8Value>
{
public:
	typedef V8Value (T::*Function)(const v8::Arguments& args);

	FuncV8(Function function)
    : function_(function), TAccessorFunction<T, V8Value>(0) {}

	virtual v8::Handle<v8::Value> call(const v8::Arguments& args);

private:
  Function function_;
};

//---------------------------------------------------------------------------
template<class T>
class ProxyClass
{
public:
	/**
   * init can be called multiple times but the name must be
   * same as the first call for this template.
   */
  ProxyClass<T>* init(char*name);

  /** returns the singleton instance for the template class */
  static ProxyClass<T>* instance() { return &proxyClass; }

  /** Expose class property member to JavaScript */
	template<class TProperty>
	void Expose( TProperty T::*property, char* propertyName,
               bool getter, bool setter);

  /** Expose class property member to JavaScript whit a custon accessor */
	template<class TProperty>
	void Expose( TProperty T::*property, char* propertyName,
               AccessorProperty* accessor, bool getter, bool setter);

  /** Expose class method member to JavaScript, can return any type and
   *  has no parameters.
   *
   *  BE AWARE OF:
   *    IF the method returns TClass* the object WILL BE DISPOSE
   *    when the garbage collector reclain the memory.
   *
   *    IF the method returns TClass& the object WILL BE NOT DESTROY
   *    by the garbage collector.
   */
  template<class TClass>
  void Expose(TClass (T::*function)(), char* functionName);

  void Expose(Handle<Value> (T::*Method) (const Arguments& args), char* functionName);

  void Expose(AccessorFunction* accessorFunction, char* functionName);

  T* UnwrapObject(Local<Object> holder);
  T* UnwrapObject(Local<Value> value);

  Handle<Value> ExposeObject(T* t, bool withDestroy);
  void ExposeObject(v8::Handle<v8::ObjectTemplate> context, T* t, const char* name, bool withDestroy);

  Handle<FunctionTemplate> GetFunctionTemplate() { return functionTemplate_; };

private:
  /** Singleton ProxyClass<T> */
	ProxyClass() {};

  /** Singleton JavaScript name */
  Local<String> className_;

  /** List of accessors, this is used to dispose accessors when the template
   *  is destroy
   */
  internalUtil::AccessorList accessorList_;

  /** V8 FunctionTemplate */
	Handle<FunctionTemplate> functionTemplate_;

  /** V8 ObjectTemplate */
	Handle<ObjectTemplate> objectTemplate_;

  /** static single class ProxyClass<T> */
  static ProxyClass<T> proxyClass;
};

}

/**
* Handy macro to get the proxy class
*
* \code
*    ProxyClass<Point>* cPoint = CPROXYV8_CLASS(Point);
* \endcode
*/
#define CPROXYV8_CLASS(CLASS) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#CLASS"")

/**
* Handy macro to get the proxy class
*
* \code
*    ProxyClass<Point>* cPoint = CPROXYV8_CLASS(Point);
* \endcode
*/
#define CPROXYV8_CLASS_ID(CLASS,ID) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#ID"")


/**
* Handy macro to expose properties of the ProxyClass<T> with the same ID in JS
*
* \code
*    CPROXYV8_PROPERTY(Point, x, true, true);
* \endcode
*/
#define CPROXYV8_PROPERTY(CLASS, PROPERTY, GET, SET) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#CLASS""); \
  CProxyV8::ProxyClass<CLASS>::instance()->Expose(&CLASS::PROPERTY, ""#PROPERTY"", SET, GET)

/**
* Handy macro to expose properties of the ProxyClass<T> with different ID in JS
*
* \code
*    CPROXYV8_PROPERTY_ID(Point, x, x_, true, true);
* \endcode
*/

#define CPROXYV8_PROPERTY_ID(CLASS, PROPERTY, ID, GET, SET) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#CLASS""); \
  CProxyV8::ProxyClass<CLASS>::instance()->Expose(&CLASS::PROPERTY, ""#ID"", SET, GET)

/**
* Handy macro to expose methods of the ProxyClass<T> with the same ID in JS
*
* \code
*    CPROXYV8_METHOD(Point, translate);
* \endcode
*/
#define CPROXYV8_METHOD(CLASS, METHOD) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#CLASS""); \
  CProxyV8::ProxyClass<CLASS>::instance()->Expose(&CLASS::METHOD, ""#METHOD"")

/**
* Handy macro to expose methods of the ProxyClass<T> with different ID in JS
*
* \code
*    CPROXYV8_METHOD_ID(Point, Translate, translate);
* \endcode
*/
#define CPROXYV8_METHOD_ID(CLASS, METHOD, ID) \
  CProxyV8::ProxyClass<CLASS>::instance()->init(""#CLASS""); \
  CProxyV8::ProxyClass<CLASS>::instance()->Expose(&CLASS::METHOD, ""#ID"")

namespace CProxyV8
{
  using namespace v8;
/**
 * Type::Cast is a facade pattern, and provides a simplified interface between C++ and
 *            JavaScript.
 *
 * This encapsulate the cast between objects from JS->C++ and from C++->JS
 *
 * The only down side of this is when casting from JS to C++, it requires pointer
 * to the target type, this is necessary to give to the compiler the information about
 * witch Cast function needs to be call.
 *
 * If a new primitive needs to be added here is where the cast need to be added.
 */

namespace Type
{
  /** Constructor C++ */
  template <class T>
  inline T* New(T*t, const v8::Handle<v8::Object>& args = v8::Handle<v8::Object>())
  {
    ASSERT(!t);
	if(args.IsEmpty())
		return new T();

    return new T(args);
  }

  /** Cast from V8 to C++ */
  inline bool Cast(bool*, Local<Value> value)
  {
    return value->BooleanValue();
  }

  inline double Cast(double*, Local<Value> value)
  {
    return value->NumberValue();
  }

  inline uint32_t Cast(uint32_t*, Local<Value> value)
  {
    return value->Uint32Value();
  }

  inline int32_t  Cast(int32_t*, Local<Value> value)
  {
    return value->Int32Value();
  }

  inline int64_t  Cast(int64_t*, Local<Value> value)
  {
    return value->IntegerValue();
  }

  inline Handle<Value> Cast(Handle<Value> value)
  {
    return value;
  };


  /** Cast V8:Value to primitive value */
  template<class T>
  inline T Cast(T*, Local<Value> value)
  {
    return ProxyClass<T>::instance()->UnwrapObject(value);
  }

  template<class T>
  inline T Cast(T&, Local<Value> value)
  {
    return ProxyClass<T>::instance()->UnwrapObject(value);
  }

  /** Cast from C++ to V8 */
  inline Handle<Value> Cast(bool caseParam)     { return Boolean::New(caseParam); }
  inline Handle<Value> Cast(double caseParam)   { return Number::New(caseParam); }
  inline Handle<Value> Cast(uint32_t caseParam) { return Uint32::New(caseParam); }
  inline Handle<Value> Cast(int32_t  caseParam) { return Int32::New(caseParam); }

  template<class T>
  inline Handle<Value> Cast(T* t)
  {
    return ProxyClass<T>::instance()->ExposeObject(t, false);
  };

  template<class T>
  inline Handle<Value> Cast(T& t)
  {
    return ProxyClass<T>::instance()->ExposeObject(&t, false);
  };

  /** Function return value Cast from C++ to V8 */
  inline Handle<Value> FCast(bool caseParam)     { return Cast(caseParam); }
  inline Handle<Value> FCast(double caseParam)   { return Cast(caseParam); }
  inline Handle<Value> FCast(uint32_t caseParam) { return Cast(caseParam); }
  inline Handle<Value> FCast(int32_t  caseParam) { return Cast(caseParam); }

  template<class T>
  inline Handle<Value> FCast(T* t)
  {
    return ProxyClass<T>::instance()->ExposeObject(t, true);
  };

  template<class T>
  inline Handle<Value> FCast(T& t)
  {
    return ProxyClass<T>::instance()->ExposeObject(&t, true);
  };

};

//Setter implementation for object properties
template<class T, class TProperty>
void ObjectProperty<T,TProperty>::AccessorSetProperty( v8::Local<v8::String> property,
                                                       v8::Local<v8::Value> value,
                                                       const v8::AccessorInfo& info)
{
	T* t = ProxyClass<T>::instance()->UnwrapObject(info.Holder());
  t->*propertyDir_ = Type::Cast((TProperty*) NULL, value);
}

//Getter implementation for object properties
template<class T, class TProperty>
v8::Handle<v8::Value> ObjectProperty<T,TProperty>::AccessorGetProperty(v8::Local<v8::String> property,
                                                                       const v8::AccessorInfo& info)
{
  T* t = ProxyClass<T>::instance()->UnwrapObject(info.Holder());
  return Type::Cast((t->*propertyDir_));
}

template<class T>
v8::Handle<v8::Value> FuncV8<T>::call(const v8::Arguments& args)
{
  T* t = ProxyClass<T>::instance()->UnwrapObject(args.Holder());
  return Type::Cast((t->*function_)(args));
}



//-----------------------------------------------------------------------------
//-------------------------------------------------------------------- Property
//-----------------------------------------------------------------------------

// A smallish abstract interface class that will make me feel comfortable
// when downcasting to InstaceHandle<T>. This interface has a method for asking
// the type info
//

template <class T>
class InstaceHandle;

class InstaceHandleBase
{
public:
	virtual const char * GetType() = 0;

	template <class T>
	bool Is()
	{
		std::string _1 = typeid(reinterpret_cast<T*>(4096)).name();
		std::string _2 = GetType();
		return _1 == _2;
	}

	// Call if 100% positively absolutely sure that this object contains the type T
	// (By checking with Is or GetType)
	template <class T>
	T* GetObject()
	{
		if(!Is<T>())return NULL;
		return ** static_cast<CProxyV8::InstaceHandle<T>*>(this);
	}
};

template <class T>
class InstaceHandle : public InstaceHandleBase
{
public:
  InstaceHandle(bool toBeCreated)
    : t_(NULL), toBeDestroy_(toBeCreated),toBeCreated_(toBeCreated) {};

  explicit InstaceHandle(T* t)
    : t_(t), toBeDestroy_(false), toBeCreated_(false) {}

  ~InstaceHandle() { if (t_ && toBeDestroy_) delete t_; args_.Dispose(); }

  inline void set(T*t, bool toBeDestroy);

  // implementation of GetType

  const char* GetType()
  {
	// only the type matters, not the instance itself.
	// excuse this hackery. (remember, t_ could be null and typeid doesn't like that)
	return typeid(reinterpret_cast<T*>(4096)).name();
  }

  /**
   * Returns true if the handle is empty.
   */
  inline bool IsEmpty();

  T* operator->();

  T* operator*();

  /**
   * Checks whether two handles are the same.
   * Returns true if both are empty, or if the objects
   * to which they refer are identical.
   * The handles' references are not checked.
   */
  template <class S> bool operator==(Handle<S> that);

  /**
   * Checks whether two handles are different.
   * Returns true if only one of the handles is empty, or if
   * the objects to which they refer are different.
   * The handles' references are not checked.
   */
  template <class S> bool operator!=(Handle<S> that)
  {
    return !operator==(that);
  }

  static InstaceHandle<T>* Cast(void* that)
  {
    return static_cast<InstaceHandle<T>*>(that);
  }

  /*
	Constructor parameters are stashed here so that they
	can be forwarded to the object once its created */
  void setArguments(const v8::Arguments& args);

private:
  T* t_;
  bool toBeDestroy_;
  bool toBeCreated_;
  v8::Persistent<v8::Object> args_;
};

template <class T>
T* InstaceHandle<T>::operator->()
{
  if (!t_ && toBeCreated_)
  {
    t_ = Type::New((T*)NULL, args_);
    toBeCreated_ = false;
  }

  return t_;
}

template <class T>
void InstaceHandle<T>::set(T*t, bool toBeDestroy)
{
  if (t_ && toBeDestroy_)
    delete t_;
  toBeDestroy = toBeDestroy;
  t_ = t;
}

template <class T>
template <class S>
bool InstaceHandle<T>::operator==(Handle<S> that)
{
  if (!t_ && toBeCreated_)
    return false;

  void** a = reinterpret_cast<void**>(**this);
  void** b = reinterpret_cast<void**>(*that);
  if (a == 0) return b == 0;
  if (b == 0) return false;
  return *a == *b;
}


template <class T>
bool InstaceHandle<T>::IsEmpty()
{
    return t_ == 0 && !toBeCreated_;
}

template <class T>
T* InstaceHandle<T>::operator*()
{
  if (!t_ && toBeCreated_)
  {
    t_ = Type::New((T*)NULL, args_);
    toBeCreated_ = false;
  }

  return t_;
}

void args2vector(const v8::Arguments& in, v8::Handle<v8::Object>& out);

template <class T>
void InstaceHandle<T>::setArguments(const v8::Arguments& args)
{
	args_.Clear();
	if(args.Length() > 0)
	{
		args2vector(args, args_);
	}
}


template<class T>
ProxyClass<T> ProxyClass<T>::proxyClass;

template<class T>
Handle<Value> Create(const Arguments& args);

template<class T>
ProxyClass<T>* ProxyClass<T>::init(char* name)
{
  if (!className_.IsEmpty())
  {
    String::Utf8Value currentName(className_);
    //If the name is the same just return the instance
    if (strcmp(*currentName,name))
    {
      //Utils::ReportApiFailure("cProxyV8::ProxyClass", "Error initializing - Class already exist");
      return NULL;
    }
  }
  else
  {
    className_ = String::New(name);

	functionTemplate_ = FunctionTemplate::New(/*Create<T>*/);
	  functionTemplate_->SetClassName(className_);

	  // Instance template from which Point objects are based on
	  objectTemplate_ = functionTemplate_->InstanceTemplate();
	  objectTemplate_->SetInternalFieldCount(1); // create an internal field for the C++ object
  }

  return instance();
}

template<class T>
template<class TProperty>
void ProxyClass<T>::Expose( TProperty T::*property, char* propertyName,
                            bool getter, bool setter)
{
  ObjectProperty<T,TProperty>* accessor = new ObjectProperty<T,TProperty>(property);
  accessorList_.push(accessor);
  Expose(property, propertyName, accessor, getter, setter);
}

template<class T>
template<class TProperty>
void ProxyClass<T>::Expose( TProperty T::*property, char* propertyName,
                            AccessorProperty* accessor, bool getter, bool setter)
{
  ASSERT(propertyName);
  ASSERT(setter || getter);

	AccessorGetter getterAccessor = getter ? Accessor_GetProperty : 0;
	AccessorSetter setterAccessor = setter ? Accessor_SetProperty : 0;
  Handle<Value> data = External::New(accessor);

	objectTemplate_->SetAccessor(String::New(propertyName), getterAccessor, setterAccessor, data);
}
/***/

template<class T>
template<class TClass>
void ProxyClass<T>::Expose(TClass (T::*function)(), char* functionName)
{
  AccessorFunction* accessor = new FuncRetObj<T,TClass>(function);
  accessorList_.push(accessor);
  Expose(accessor, functionName);
}

template<class T>
void ProxyClass<T>::Expose(Handle<Value> (T::*function) (const Arguments& args),
                           char* functionName)
{
  AccessorFunction* accessor = new FuncV8<T>(function);
  accessorList_.push(accessor);
  Expose(accessor, functionName);
}

template<class T>
void ProxyClass<T>::Expose(AccessorFunction* accessorFunction, char* functionName)
{
  Handle<Value> data = External::New(accessorFunction);
  Local<FunctionTemplate> func = FunctionTemplate::New();
  func->SetCallHandler(Accessor_Function,data);
  objectTemplate_->Set(functionName, func);
}

template<class T>
T* ProxyClass<T>::UnwrapObject(Local<Object> holder)
{
  Local<External> external = Local<External>::Cast(holder->GetInternalField(0));
  InstaceHandle<T>* tHandle = InstaceHandle<T>::Cast(external->Value());
  return *(*tHandle);
}

template<class T>
T* ProxyClass<T>::UnwrapObject(Local<Value> value)
{
  Local<External> external = Local<External>::Cast(value);
  InstaceHandle<T>* tHandle = static_cast<InstaceHandle<T>*>(external->Value());
  return *(*tHandle);
}

template<class T>
void ProxyClass<T>::ExposeObject(v8::Handle<v8::ObjectTemplate> context, T* t, const char* name, bool withDestroy)
{
  HandleScope handle_scope;
  context->Set(String::New(name), ExposeObject(t, withDestroy));
}

//-----------------------------------------------------------------------------
//--------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class T>
Handle<Value> ProxyClass<T>::ExposeObject(T* t, bool withDestroy)
{
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  Handle<ObjectTemplate> templ = objectTemplate_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Local<External> externalVal = Local<External>::Cast(result->GetInternalField(0));

  InstaceHandle<T>* tHandle = static_cast<InstaceHandle<T>*>(externalVal->Value());
  tHandle->set(t, withDestroy);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

template<class T>
void Class_Destroy(Persistent<Object> self, void* parameter);
// js T constructor function, called when `new T(...)' in js
template<class T>
Handle<Value> Create(const Arguments& args)
{
	// throw if called without `new'
	if (!args.IsConstructCall())
		return ThrowException(String::New("Cannot call constructor as function"));

	// throw if we didn't get 0 args
	// Commented out by Jontte: Why shouldn't we allow constructor parameters?
	/*if (args.Length() != 0)
		return ThrowException(String::New("Expected no arguments"));
	*/
	// create the C++ Handle<T> to be wrapped
	InstaceHandle<T>* t = new InstaceHandle<T>(true);
	t->setArguments(args);

	// make a persistent handle for the instance and make it
	// weak so we get a callback when it is garbage collected
	Persistent<Object> self = Persistent<Object>::New(args.Holder());
	self.MakeWeak(t, WeakReferenceCallback(Class_Destroy<T>));

	// set internal field to the C++ point
	self->SetInternalField(0, External::New(t));

	// My addition: For various reasons it will help to create the underlying object right-away..
	**t;

	return self;
}

/**
 * Called when the T object is being garbage collected
 * delete the C++ object and ClearWeak on the Persistent handle.
 */
template<class T>
void Class_Destroy(Persistent<Object> self, void* parameter)
{
	delete static_cast<T*>(parameter);
	self.ClearWeak();
}

}
#endif
