#include "StdAfx.h"
#include "Util.h"

bool readFile(
	IrrlichtDevice* irr, 
	const std::string& path, 
	std::string& result)
{
	intrusive_ptr<io::IReadFile> file(irr->getFileSystem()->createAndOpenFile(path.c_str()), false);

	if(!file.get())
		return false;
	result.resize(file->getSize());

	int bytesleft = file->getSize();
	int bytesread = 0;
	while(bytesleft > 0)
	{
		int bs = file->read(&result[bytesread], bytesleft);

		bytesread += bs;
		bytesleft -= bs;
	}
	return true;
}

void btTransformToIrrlichtMatrix(const btTransform& worldTrans, irr::core::matrix4 &matr) 
{ 
    worldTrans.getOpenGLMatrix(matr.pointer()); 
} 


void btTransformFromIrrlichtMatrix(const irr::core::matrix4& irrmat, btTransform &transform) 
{ 
    transform.setIdentity(); 

    transform.setFromOpenGLMatrix(irrmat.pointer()); 
}

