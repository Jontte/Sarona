#include "StdAfx.h"
#include "Client.h"
#include "JSObject.h"
#include "PhysWorld.h"
#include "PhysObject.h"

#include "JSConvert.h"
#include "PacketDef.h"

namespace Sarona
{
void Client::SetCamera(v8::Handle<v8::Value> options)
{
	using namespace extractor;
	// Make player camera follow some object
	// Convert to Native types

	try
	{
		string type;
		emap(options)
			("type", type, REQUIRED);

		if(type == "chase")
		{
			v8::Handle<v8::Object> target;
			double distance = 0;
			double height = 0;
			bool follow_pitch = true;
			emap(options)
				("target", 			target, 		REQUIRED)
				("distance", 		distance, 		REQUIRED)
				("height", 			height, 		0)
				("follow_pitch", 	follow_pitch, 	0);

			JSObject * target_obj = v8::juice::cw::ClassWrap<JSObject>::ToNative::Value(target);
			if(!target_obj)
				return;
			PhysObject* physobj = target_obj->getObject();
			if(!physobj)
				return;

			ZCom_NodeID	id = physobj->getNetworkId();

			// Send cameraFollow packet...
			ZCom_BitStream * stream = new ZCom_BitStream;

			Protocol::SetCameraChase chase;
			chase.nodeid = id;
			chase.distance = distance;
			chase.height = height;
			chase.follow_pitch = follow_pitch;
			chase.write(*stream);

			m_world->sendEventDirect(eZCom_ReliableOrdered, stream, m_connection_id);
			return;
		}
		else if(type == "position")
		{

		}
		else if(type == "free")
		{

		}
		else
		{

		}
	}
	catch(...)
	{
		v8::ThrowException(v8::String::New("Invalid arguments"));
		return;
	}
	v8::ThrowException(v8::String::New("Invalid arguments"));
	return;
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

	b.Set("setCamera", ICM::M1::Invocable<void, v8::Handle<v8::Value>, &Client::JSHandle::SetCamera>);

	// with or without context
	b.Set("bind", ICM::M3::Invocable<void, string, string, v8::Handle<v8::Value>, &Client::JSHandle::BindEvent>);
//	b.Set("bind", ICM::M4::Invocable<void, string, string, v8::Handle<v8::Value>, v8::Handle<v8::Value>, &Client::JSHandle::BindEvent>);

	b.Seal();
	b.AddClassTo(dest);
	return b.CtorTemplate()->GetFunction();
}

}
