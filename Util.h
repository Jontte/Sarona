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


string keycode2string(irr::u8 code);


#endif
