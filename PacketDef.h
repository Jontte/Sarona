#pragma once
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
		template<unsigned Id> struct PID {
			static const int ID = Id;
		};

		// Macro hackery
		#define FIELDDEF(AUX, DATA, ELEM) BOOST_PP_TUPLE_ELEM(2, 0, ELEM) BOOST_PP_TUPLE_ELEM(2, 1, ELEM) ;
		#define SERIALIZEDEF(AUX, DATA, ELEM) DATA & BOOST_PP_TUPLE_ELEM(2, 1, ELEM) ;

		#define DEF(ID, NAME, FIELDS) \
		struct NAME : public PID<ID> \
		{\
			BOOST_PP_SEQ_FOR_EACH(FIELDDEF, _, FIELDS)\
			template<class Archive> \
			void serialize(Archive & ar) \
			{ \
				BOOST_PP_SEQ_FOR_EACH(SERIALIZEDEF, ar, FIELDS)\
			} \
		};

		// - - - - - - - - - - P A C K E T   D E F I N I T I O N S - - - - - - - - - -
		// Namespaces :
		// 0x0000 - Connection management packages
		// 0x0100 - World updates
		// 0x0200 - Events

		DEF(0x0000, VersionCheck,		((s16, major)) ((s16, minor)));
		DEF(0x0001, LevelHashCheck,		((string, sha1)));

		DEF(0x0100, PositionUpdate,	
			((	u32	, object_id						))	
			((	s32, x  ))(( s32, y  ))(( s32, z	))	// Position
			((	s32, vx ))(( s32, vy ))(( s32, vz	))  // Velocity
				// TODO: Orientation, Omega...
			);

		DEF(0x0200, GlobalEvent,		((s32, message_id))((string, message)));
		DEF(0x0201, QuickEvent,			((s32, object_id))((string, message)));


		// Utility functions
/*
		// Check if raw data contains a packet
		template <class PacketClass>
		bool PacketIs(byte*& data, int& count)
		{
			if(count <= 2)return false;

			u16& id = PacketClass::ID;
			union{char in[2], u16 out};
			in[0]=data[0];
			in[1]=data[1];
			if(out == id)
			{
				// ATN: shifts pointer
				data+=2;
				return true;
			}
			return false;
		}

		template <class PacketClass>
		u16 PacketID(const PacketClass& p)
		{
			return PacketClass::ID;
		}
*/
		// Cleanup..
		#undef DEF
		#undef FIELDDEF
		#undef SERIALIZEDEF
	}
}

