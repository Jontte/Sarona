#pragma once
#ifndef UTIL_H_
#define UTIL_H_
#include "StdAfx.h"

void intrusive_ptr_add_ref(IReferenceCounted* t);
void intrusive_ptr_release(IReferenceCounted* t);

bool readFile(
	IrrlichtDevice* irr,
	const std::string& path,
	std::string& result);

void btTransformToIrrlichtMatrix(const btTransform& worldTrans, irr::core::matrix4 &matr) ;
void btTransformFromIrrlichtMatrix(const irr::core::matrix4& irrmat, btTransform &transform) ;

// Adds support for loading 3d models from 2d textures
void addHeightMapLoader(IrrlichtDevice* device);

string keycode2string(irr::u8 code);

/*
	A simple timer class that uses boost::posix_time
	boost::timer uses clock() which makes it practically useless
*/
class SimpleTimer
{
	private:
		boost::posix_time::ptime m_start;
	public:
		SimpleTimer();
		void restart();
		// if parameter is given, return time in seconds between restart and that time
		double elapsed(const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::local_time());
};


// A simple macro to time function calls
#define PROFILE(NAME, ...)\
	SimpleTimer NAME ## _profiler_;\
	__VA_ARGS__;\
	std::cout << "PROFILER: " << BOOST_PP_STRINGIZE(NAME) << " = " << NAME ## _profiler_.elapsed() << " s" << std::endl;


#endif
