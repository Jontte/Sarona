#pragma once
#ifndef PACKETDEF_H_
#define PACKETDEF_H_
#include "StdAfx.h"
#include "Util.h"
/*
	This file defines all the packets used by the protocol

	Each message begins with a two-byte id denoting the packet id
*/

namespace Sarona
{
	namespace Protocol
	{
		// Protocol type type traits
		// Currently field widths are hard coded...
		template <class T> struct TypeDefinition;

#define TYPE_DEFINITION_CUSTOM(TYPE, READ, WRITE) \
		template <> struct TypeDefinition< TYPE > {\
			static inline TYPE read(ZCom_BitStream & _stream) { \
				READ;\
			} \
			static inline void write(ZCom_BitStream & _stream, const TYPE& data) { \
				WRITE;\
			} \
		};
#define TYPE_DEFINITION(TYPE, READ, WRITE) \
		TYPE_DEFINITION_CUSTOM(TYPE, \
		return _stream.READ; ,\
		_stream.WRITE ;);\

		// Macro hackery
		#define FIELDDEF(AUX, DATA, ELEM) BOOST_PP_TUPLE_ELEM(2, 0, ELEM) BOOST_PP_TUPLE_ELEM(2, 1, ELEM) ;
		#define READDEF(AUX, DATA, ELEM) BOOST_PP_TUPLE_ELEM(2, 1, ELEM) = TypeDefinition<BOOST_PP_TUPLE_ELEM(2, 0, ELEM)>::read(DATA);
		#define WRITEDEF(AUX, DATA, ELEM) TypeDefinition<BOOST_PP_TUPLE_ELEM(2, 0, ELEM)>::write(DATA, BOOST_PP_TUPLE_ELEM(2, 1, ELEM));

		#define DEF(ID, NAME, FIELDS) \
		struct NAME\
		{\
			static const unsigned int Id = ID;\
			BOOST_PP_SEQ_FOR_EACH(FIELDDEF, _, FIELDS)\
			void read(ZCom_BitStream & _stream) \
			{ \
				BOOST_PP_SEQ_FOR_EACH(READDEF, _stream, FIELDS)\
			} \
			void write(ZCom_BitStream & _stream) \
			{ \
				_stream.addInt(Id, 16); /*add packet id*/ \
				BOOST_PP_SEQ_FOR_EACH(WRITEDEF, _stream, FIELDS)\
			} \
		};\
		namespace UniqueId {\
			typedef NAME ensure_unique_id_ ## ID;\
		}

		// - - - - - - - - - - T Y P E   D E F I N I T I O N S - - - - - - - - - -
		// TYPE_DEFINITION(type, reader function, writer function)
		TYPE_DEFINITION(u8,  getInt(8), addInt(data, 8));
		TYPE_DEFINITION(u16, getInt(16), addInt(data, 16));
		TYPE_DEFINITION(s16, getSignedInt(16), addSignedInt(data, 16));
		TYPE_DEFINITION(u32, getInt(32), addInt(data, 32));
		TYPE_DEFINITION(s32, getSignedInt(32), addSignedInt(data, 32));
		TYPE_DEFINITION(bool,  getBool(), addBool(data));
		TYPE_DEFINITION(float, getFloat(23), addFloat(data, 23));
		TYPE_DEFINITION(string, getStringStatic(), addString(data.c_str()));

		TYPE_DEFINITION_CUSTOM(btVector3,
			return btVector3(_stream.getFloat(23),_stream.getFloat(23),_stream.getFloat(23)),
			_stream.addFloat(data[0], 23); _stream.addFloat(data[1], 23); _stream.addFloat(data[2], 23);
		);

		// - - - - - - - - - - P A C K E T   D E F I N I T I O N S - - - - - - - - - -
		// Namespaces :
		// 0x0000 - Connection management packages
		// 0x0100 - Object-level events, Client -> Server
		// 0x0200 - Object-level events, Server -> Client
		// 0x0300 - Global events, Client -> Server
		// 0x0400 - Global events, Server -> Client

		DEF(0x0000, VersionCheck,		((s16, major)) ((s16, minor)))
		DEF(0x0001, LevelHashCheck,		((string, sha1)))

		DEF(0x0200, NodeStateUpdate, ((string, mesh))((string, texture))((btVector3, mesh_scale)))

		DEF(0x0300, KeyPressEvent, ((u8, keycode))((bool, press)))
		DEF(0x0301, LevelRequest, ((string, name)))  // Client must send either of these two as a reply to LevelSelect
		DEF(0x0302, LevelConfirm, ((bool, unused)))

		DEF(0x0400, LevelSelect, ((string, name)))
		DEF(0x0401, GameStartNotify, ((bool, unused)))

		// Camera setter packets
		DEF(0x0402, SetCameraChase, ((u32, nodeid))((float, distance))((float, height))((bool, follow_pitch)));
		DEF(0x0403, SetCameraPos, ((btVector3, position_begin))((btVector3, position_end))((btVector3, lookat_begin))((btVector3, lookat_end))((float, seconds)));
		DEF(0x0404, SetCameraFree, ((bool, unused)));

		// Cleanup..
		#undef DEF
		#undef FIELDDEF
		#undef READDEF
		#undef WRITEDEF
		#undef TYPE_DEFINITION
	}
}

#endif
