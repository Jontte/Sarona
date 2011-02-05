#include "StdAfx.h"
#include "Client.h"
#include "JSObject.h"
#include "PhysWorld.h"
#include "PhysObject.h"
#include "PacketDef.h"

namespace Sarona
{

void Client::CameraFollow(JSObject& target, double radius)
{
	// Make player camera follow some object
	// Convert to Native types

	PhysObject* physobj = target.getObject();
	if(physobj)
	{
		ZCom_NodeID	id = physobj->getNetworkId();

		// Send cameraFollow packet...
		ZCom_BitStream * stream = new ZCom_BitStream;

		Protocol::CameraFollow follow;
		follow.nodeid = id;
		follow.distance = (float)radius;
		follow.write(*stream);

		m_world->sendEventDirect(eZCom_ReliableOrdered, stream, m_connection_id);
	}
}

void Client::CameraSet(const v8::Arguments& args)
{

}

void Client::BindEvent(string type, string subtype, v8::Handle<v8::Value> func, v8::Handle<v8::Object> ctx)
{
	if(type == "" || subtype == "") return;

	// Register event
	//TODO: doesn't handle duplicates
	auto_ptr<Event> e(new Event);
	e->type = type;
	e->subtype = subtype;
	e->function = v8::Persistent<v8::Value>::New(func);
	if(!ctx.IsEmpty())
		e->context = v8::Persistent<v8::Object>::New(ctx);

	// construct key
	string key;
	key.reserve(type.length()+subtype.length()+3);
	key.append(type);
	key.append("-");
	key.append(subtype);

	m_events.insert(key, e.release());
}

void Client::CallEvent(const string& type, const string& subtype, const std::vector<v8::Handle< v8::Value > >& args)
{
	if(type == "" || subtype == "") return;

	// construct key
	string key;
	key.reserve(type.length()+subtype.length()+3);
	key.append(type);
	key.append("-");
	key.append(subtype);

	ptr_map<string, Event>::iterator iter = m_events.find(key);
	if(iter != m_events.end())
	{
		Event * evt = iter->second;
		// If no context was supplied, use the global obj
		v8::Handle<v8::Value> result = m_world->CallFunction(
				evt->function,
				vector<v8::Handle<v8::Value> >(), // no args
				evt->context);
		if(result.IsEmpty())
		{
			std::cout << "Client event call failed!" << std::endl;
		}
	}
}


v8::Handle<v8::Object> Client::JSHandle::SetupClass(v8::Handle<v8::Object> dest)
{
	// Setup JS bindings to client object
	typedef v8::juice::cw::ClassWrap<Client::JSHandle> CW;
	typedef v8::juice::convert::MemFuncInvocationCallbackCreator<Client::JSHandle> ICM;

	CW & b (CW::Instance());

	b.Set("cameraFollow", ICM::M2::Invocable<void, JSObject&, double, &Client::JSHandle::CameraFollow>);

	b.Seal();
	b.AddClassTo(dest);
	return b.CtorTemplate()->GetFunction();
}

}
