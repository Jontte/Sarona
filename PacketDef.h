#pragma once
#include "StdAfx.h"

/*
	This file defines all the packets used by the protocol

	Each message begins with a two-byte id denoting the packet id
*/

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/tracking.hpp>

#

namespace Sarona
{
	namespace Protocol
	{
		// Simple types 
		typedef uint8_t 	u8;
		typedef uint16_t	u16;
		typedef uint32_t	u32;
		typedef uint64_t	u64;

		template<unsigned Id> struct PID {
			static const int ID = Id;
		};

		// Macro hackery

		#define FIELDDEF(AUX, DATA, ELEM) BOOST_PP_TUPLE_ELEM(2, 0, ELEM) BOOST_PP_TUPLE_ELEM(2, 1, ELEM) ;
		#define SERIALIZEDEF(AUX, DATA, ELEM) DATA & BOOST_PP_TUPLE_ELEM(2, 1, ELEM) ;

		#define DEF(ID, NAME, FIELDS) \
		namespace Sarona { namespace Protocol { \
		struct NAME : public PID<ID> \
		{\
			BOOST_PP_SEQ_FOR_EACH(FIELDDEF, _, FIELDS)\
			template<class Archive> \
			void serialize(Archive & ar, const unsigned int version) \
			{ \
				BOOST_PP_SEQ_FOR_EACH(SERIALIZEDEF, ar, FIELDS)\
			} \
		}; }}\
		BOOST_CLASS_IMPLEMENTATION(Sarona::Protocol::NAME, boost::serialization::object_serializable);\
		BOOST_CLASS_TRACKING(Sarona::Protocol::NAME, boost::serialization::track_never);
		// Close namespaces temporarily BOOST_CLASS_* lines above can't stand it otherwise.
	} 
}

// - - - - - - - - - - P A C K E T   D E F I N I T I O N S - - - - - - - - - -
// Namespaces :
// 0x0000 - Connection management packages
// 0x0100 - World updates
// 0x0200 - Events

DEF(0x0000, VersionCheck,		((int, major))((int, minor)));
//DEF(0x0100, PositionUpdate,		((int, major))((int, minor)));
//DEF(0x0200, CompactEvent,		((int, major))((int, minor)));


// Reopen namespaces

namespace Sarona
{
	namespace Protocol
	{

		// Utility functions

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

		// Cleanup..
		#undef DEF
		#undef FIELDDEF
		#undef SERIALIZEDEF

	}

}

