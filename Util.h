#pragma once
#include "StdAfx.h"

bool readFile(
	IrrlichtDevice* irr, 
	const std::string& path, 
	std::string& result);

void btTransformToIrrlichtMatrix(const btTransform& worldTrans, irr::core::matrix4 &matr) ;

void btTransformFromIrrlichtMatrix(const irr::core::matrix4& irrmat, btTransform &transform) ;


string keycode2string(irr::u8 code);


