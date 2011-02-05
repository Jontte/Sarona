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

	TODO: The Client class, though, outlives the JS engine?
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


	void CameraFollow(const JSObject& target, double radius);
	void CameraSet(const v8::Arguments& args);
	void BindEvent(string type, string subtype, v8::Handle<v8::Value> func, v8::Handle<v8::Object> ctx = v8::Handle<v8::Object>());
	void CallEvent(const string& type, const string& subtype, const std::vector<v8::Handle< v8::Value > >& args = vector<v8::Handle<v8::Value> >());

	static v8::Handle<v8::Object> SetupClass(v8::Handle<v8::Object> dest);
};


class Client::JSHandle
{
	private:
		Client* m_ref;
	public:
		JSHandle():m_ref(NULL){};
		void initialize(Client* ref){m_ref = ref;}
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

#endif // CLIENT_H_
