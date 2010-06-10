#pragma once
#include "StdAfx.h"

namespace Sarona
{

	template <class WorldType, class ObjectType>
	class BaseWorld 
		: public ZCom_Control
		, public ZCom_Node
		, public ZCom_NodeEventInterceptor
	{
		typedef typename ObjectType::Id ObjectId;

	protected:
		// Level config
		string m_name;
		string m_description;
		string m_author;

		string m_levelname;

		// V8
		v8::Persistent<v8::Context>		m_jscontext;

		// Irrlicht
		intrusive_ptr<IrrlichtDevice>	m_device;

		// Networking
		ZCom_Control*					m_control;

		void RegisterZComObjects()
		{
			// has to be called after initialization, before any connections are made
			TypeRegistry::m_objectId = this->ZCom_registerClass("Object");
			TypeRegistry::m_commId = this->ZCom_registerClass("GlobalCommunicator");
		}

		void LoadLevel(bool runscripts)
		{
			if(m_levelname.empty())
			{
				throw std::invalid_argument("No level was selected!");
			}
			// Load folder
			
			bool success = m_device->getFileSystem()->addFileArchive(
				m_levelname.c_str(),
				false, /* ignore case */
				false, /* ignore paths */
				io::EFAT_FOLDER
				);

			if(! success)
			{
				throw std::invalid_argument(string("Unable to load folder ") + m_levelname);
			}

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

			if(runscripts)
			{
				Json::Value scripts = root.get("scripts", Json::nullValue);

				v8::Locker locker;
				v8::Context::Scope context_scope(m_jscontext);

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
					
					v8::Handle<v8::String> jssource = v8::String::New(source.c_str());

					v8::TryCatch trycatch;

					v8::Handle<v8::Script> jsscript = v8::Script::Compile(jssource);

					v8::Handle<v8::Value> result;

					if(!jsscript.IsEmpty())
						result = jsscript->Run();
					
					if(result.IsEmpty())
					{
						v8::Handle<v8::Value> exception = trycatch.Exception();
						v8::Handle<v8::Message> message = trycatch.Message();

						v8::String::AsciiValue exception_str(exception);

						std::cout << "Exception: " << *exception_str << std::endl;
						std::cout << "	: " << *v8::String::AsciiValue(message->GetSourceLine()) << std::endl;
					}
				}
			}
		}


		// Whether the game (physical simulation & friends) is running
		bool m_gamerunning;

	public:

		void SetLevel(std::string level)
		{
			m_levelname = level;
		}

		BaseWorld(IrrlichtDevice* dev) 
			: m_device(dev)
			, m_gamerunning(false)
		{
			RegisterZComObjects();
		}

		virtual ~BaseWorld(void)
		{
			// Get rid of v8 context
			m_jscontext.Dispose(); 
		}

		ObjectType* getObject(ObjectId id)
		{
			ZCom_Node * node = ZCom_getNode(id);
			if(node)
			{
				return reinterpret_cast<ObjectType*>(node->getUserData());
			}
			return NULL;
		}
	};

	// ZCom Node registry ids
	namespace TypeRegistry
	{
		extern int m_objectId;
		extern int m_commId;
	};
}