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


void intrusive_ptr_add_ref(IReferenceCounted* t)
{
	if(t)
		t->grab();
}
void intrusive_ptr_release(IReferenceCounted* t)
{
	if(t)
		t->drop();
}


namespace Sarona
{
	/*int serialize(const s16& in, char* bin)
	{
		if(bin)
		{
			union{char data[2]; s16 val;};
			val = htons(in);
			bin[0]=data[0];
			bin[1]=data[1];
		}
		return 2;
	}
	


	void OutputArchive::SaveTo(vector<s8>& to)
	{
		swap(m_data, to);
	}*/
}

