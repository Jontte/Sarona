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

/**
 * #if _FILE_H Directive can not be applied to templates because the
 * compiler needs all the implementation visible to expand it.
 */

#pragma once
#ifndef CLASS_H_
#define CLASS_H_
/*
#include <string.h>
#include <typeinfo>
#include <cproxyv8.h>

#include "cproxyv8-function.h"
#include "cproxyv8-property.h"

namespace CProxyV8
{
  using namespace v8;
    namespace Type {
  inline Handle<Value> FCast(bool caseParam);
  inline Handle<Value> FCast(double caseParam);
  inline Handle<Value> FCast(uint32_t caseParam);
  inline Handle<Value> FCast(int32_t  caseParam);

  template <class T>
  inline T Cast(T*, Local<Value> value);
    } }
    */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



/* this is used to convert a v8::Arguments to a vector of v8 handles to v8 values */
void args2vector(const v8::Arguments& in, v8::Handle<v8::Object>& out);

//-----------------------------------------------------------------------------
//-------------------------------------------------------------------- class
//-----------------------------------------------------------------------------

/**
 * The ProxyClass<T> have the following objectives:
 *
 *    1. Provide an EASY way to expose properties from Cpp objects to JS
 *    2. Provide an EASY way to expose methods from Cpp objects to JS
 *    3. Avoid the need of MACROS to declare types or keep tracking of all
 *       classes. The compiler must take care of this with the templates.
 *    4. Avoid the need of specifics callbacks declarations required by
 *       V8 and replace it by hidden generic callbacks directly to the
 *       object instance.
 *    5. Achive this goals without modification of V8 code.
 *
 * ProxyClass<T> is implemented with cpp templates classes.
 *
 * Every ProxyClass<T> is a singleton instance. The compiler handles
 * the resources allocation for the specific class instance. Making the
 * ProxyClass<T> usage scope independent. There is another avantage,
 * the singlenton restriction force the program to have only 1 representation
 * of the specific ProxyClass<T> in C++ and JS, this avoid confussion
 * and force good code practices.
 *
 * ProxyClass<T> does not keep any references to objects of type T.
 *
 *  IMPORTAN IMPORTANT IMPORTANT!!!!!
 *
 *  BE AWARE: There is the posibilitye to create memory problems if the
 *            T instance is dispose and there are reference to his properties
 *            in JS.
 *
 *  TODO: Do something to help to solve this problem or at least give a warning.
 *
 * The following example shows how to use a ProxyClass<T>:
 *
 * \code
 * class Point
 * {
 * 	public:
 *     ** Constructor without params is required *
 * 		Point(int px = 0, int py = 0) : x_(px), y_(py) {}
 *     Point(Point* p) : x_(p->x_), y_(p->y_) {}
 *
 * 		int x_;
 *     int y_;
 *
 *     ** Example call to function without params *
 *     int GetManhattanLength()  { return std::abs(x_) - std::abs(y_); }
 *
 *     ** this will create a JS object, with JS destructor *
 *     Point* Copy() { return new Point(); }
 *
 *     ** Example call to function with params *
 *     v8::Handle<v8::Value> Translate(const v8::Arguments& args)
 *     {
 *       // throw if we didn't get 2 args
 *       if (args.Length() != 2)
 *         return ThrowException(v8::String::New("Expected 2 arguments"));
 *
 *       x_ += args[0]->Int32Value();
 *       y_ += args[1]->Int32Value();
 *       return v8::True();
 *     };
 * };
 *
 *  **
 *  * Simple class to show how to expose object properties and methods with
 *  * reference to this properties.
 *  *
 *  *  BE AWARE: There can create memory problems if the line instance is dispose and
 *  *            there are reference to his properties in JS. Ha handle is a better
 *  *            way to expose this objects.
 *  *
 *  *
 * class Line
 * {
 * public:
 *   ** Constructor without params is required *
 *
 *   ** Example of object property, it can be manipulate directly *
 *   Point start;
 *
 *   ** this will create a JS object, without destructor and a reference to this
 *    *  end instance
 *    *
 *    *
 *   Point& GetEnd() { return end; }
 *
 * private:
 *   Point end;
 * };
 *
 *    int main(int argc, char* argv[])
 *    {
 *      ProxyClass<Point>* classPoint = PROXY_CLASS(Point);
 *      classPoint->Expose("x", &Point::x_, true, true);
 *      classPoint->Expose("y", &Point::y_, true, true);
 *
 *      ProxyClass<Line>* classLine = PROXY_CLASS(Line);
 *      classLine->Expose("start", &Line::start_, true, true);
 *      classLine->Expose("end", &Line::end_, true, true);
 *      classLine->Expose("getXDistance", &Line::GetXDistance);
        classLine->Expose("getYDistance", &Line::GetYDistance);
 *     ....
 *
 *      v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
 *
 *      global->Set(v8::String::New("Point"), classPoint->GetFunctionTemplate()); // add Point object
 *      global->Set(v8::String::New("Line"), classLine->GetFunctionTemplate()); // add Line object
 *     ....
 *    }
 *
 *    //--- JS code
 *    > point = new Point();
 *    > line = new Line();
 *    > line.getXDistance();
 *    0
 *    > line.start.x = 10;
 *    > line.getXDistance();
 *    10
 *    >
 *
 * \endcode
 *
 */
namespace CProxyV8
{

//class AccessorProperty;



}

#endif
