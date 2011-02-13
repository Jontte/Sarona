#pragma once
#ifndef CLIENT_H_
#define CLIENT_H_
#include "StdAfx.h"

namespace Sarona
{

class JSObject;
class PhysWorld;

/*
	The Client class is created, owned and deleted by PhysWorld
	Client::JSHandle is a helper class owned by the JS engine and
	provides weak-ref access to the real Client class.

	TODO: The Client class, though, outlives the JS engine? always?
*/
class Client
{
	friend class PhysWorld;
	public:
	class JSHandle;
	private:

	struct Event
	{
		string type;
		string subtype;
		v8::Persistent<v8::Value> function; // string or function
		v8::Persistent<v8::Object> context;
		~Event() {function.Dispose(); context.Dispose();}
	};
	ptr_map<string, Event> m_events;

	public:
	string m_name;
	ZCom_ConnID m_connection_id;
	bool m_level_confirmed;
	bool m_disconnected;

	PhysWorld* m_world;

	Client(PhysWorld* world_) :
		m_name("Unnamed"),
		m_connection_id(0),
		m_level_confirmed(false),
		m_disconnected(true),
		m_world(world_){}


	void CallEvent(const string& type, const string& subtype, const std::vector<v8::Handle< v8::Value > >& args = vector<v8::Handle<v8::Value> >());

	// JS Side:

	void SetCamera(v8::Handle<v8::Value> options);
	void BindEvent(string type, string subtype, v8::Handle<v8::Value> func, v8::Handle<v8::Object> ctx = v8::Handle<v8::Object>());
};


class Client::JSHandle
{
	private:
		Client* m_ref;
	public:
		JSHandle():m_ref(NULL){};
		void initialize(Client* ref){m_ref = ref;}

		// wrapper functions that direct the calls to the native object
		// throw exception if handle is invalid for some reason
		// TODO: this copypaste mess could be cleaned with some template/macro hackery?
		void SetCamera(v8::Handle<v8::Value> val)
		{
			if(!m_ref)
				throw std::string("Invalid client handle");
			return m_ref->SetCamera(val);
		}
		void BindEvent(string type, string subtype, v8::Handle<v8::Value> func, v8::Handle<v8::Value> ctx)
		{
			if(!m_ref)
				throw std::string("Invalid client handle");
			if(!ctx->IsObject())
				throw std::string("Invalid context passed");
			return m_ref->BindEvent(type, subtype, func, ctx->ToObject());
		}
		void BindEvent(string type, string subtype, v8::Handle<v8::Value> func)
		{
			if(!m_ref)
				throw std::string("Invalid client handle");
			return m_ref->BindEvent(type, subtype, func, v8::Handle<v8::Object>());
		}

		static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
};

}

// Class Name
JUICE_CLASSWRAP_CLASSNAME(Sarona::Client::JSHandle, "Client");

// Ctor binding
namespace v8{namespace juice{namespace cw{
	template <>
	struct Factory<Sarona::Client::JSHandle> : Factory_CtorForwarder<Sarona::Client::JSHandle,
		tmp::TypeList<
			convert::CtorForwarder0<Sarona::Client::JSHandle>
		>
	>
	{};
}}}

#define CLASSWRAP_BOUND_TYPE Sarona::Client::JSHandle
#include <v8/juice/ClassWrap_TwoWay.h>

#endif // CLIENT_H_
