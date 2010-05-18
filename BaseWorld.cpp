#include "StdAfx.h"
#include "BaseWorld.h"
#include "Util.h"

namespace Sarona
	{
	BaseWorld::BaseWorld(IrrlichtDevice* dev, NetworkCtx* netctx) : m_device(dev), m_netctx(netctx)
	{
	}

	BaseWorld::~BaseWorld(void)
	{
		// Get rid of v8 context
		m_jscontext.Dispose(); 
	}

	void BaseWorld::LoadLevel()
	{
		// Read index json..
		
		std::string json_source;
		readFile(get_pointer(m_device), "index.json", json_source);

		// Parse index json..
		Json::Value root;   
		Json::Reader reader;
		if ( !reader.parse( json_source, root ) )
		{
			// report to the user the failure and their locations in the document.
			std::cout  << "Failed to parse configuration\n"
					   << reader.getFormatedErrorMessages();
			throw std::invalid_argument("Parse error");
		}

		m_name = root.get("name", "name missing").asString();
		m_description = root.get("description", "description missing").asString();
		m_author = root.get("author", "author missing").asString();

		Json::Value scripts = root.get("scripts", Json::nullValue);
		for(unsigned index = 0; index < scripts.size(); ++index)
		{
			// Read script source into string

			std::string source;

			if(!readFile(get_pointer(m_device), scripts[index].asString().c_str(), source))
			{
				// Read failed..
				continue;
			}
			

			v8::HandleScope handle_scope;
			v8::Context::Scope context_scope(m_jscontext);
			
			v8::Handle<v8::String> jssource = v8::String::New(source.c_str());

			v8::Handle<v8::Script> jsscript = v8::Script::Compile(jssource);

			v8::TryCatch trycatch;

			v8::Handle<v8::Value> result = jsscript->Run();
			
			if(result.IsEmpty())
			{
				v8::Handle<v8::Value> exception = trycatch.Exception();
				v8::String::AsciiValue exception_str(exception);

				std::cout << "Exception: " << *exception_str << std::endl;
			}
		}
	}

}