#pragma once
#include "StdAfx.h"

bool readFile(
	IrrlichtDevice* irr, 
	const std::string& path, 
	std::string& result);

void btTransformToIrrlichtMatrix(const btTransform& worldTrans, irr::core::matrix4 &matr) ;

void btTransformFromIrrlichtMatrix(const irr::core::matrix4& irrmat, btTransform &transform) ;



namespace Sarona
{
	// Simple types 
	typedef char 	s8;
	typedef int16_t	s16;
	typedef int32_t	s32;
	typedef int64_t	s64;

	template <class T>
	int serialize(const T& data, char* bin = NULL);
	int serialize(const s16&, char* bin=NULL);

	template <class T>
	int deserialize(const T& data, char* bin, int bytesleft);


	// unpack Binary format to data
	class InputArchive
	{
	private:
		vector<s8> m_data;
		int m_cursor;
	public:
		InputArchive(vector<s8> data);
		template <class T>
		void operator&(const T& other)
		{
			int len = deserialize(other, &m_data[m_cursor], m_data.size()-m_cursor);
			m_cursor += len;
		}
	};

	// pack data to Binary format
	class OutputArchive
	{
	private:
		vector<s8> m_data;
	public:
		void SaveTo(vector<s8>& data);
		template <class T>
		void operator&(const T& other)
		{
			int len = serialize(other);
			// grow vector
			m_data.resize(m_data.size() + len);
			serialize(other, &m_data[m_data.size()-len]);
		}
	};
}
