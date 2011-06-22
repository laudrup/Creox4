/***************************************************************************
					crdatastream.h  -  description
						 -------------------
		begin                : Sun May 20 2001
		copyright            : (C) 2001 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
                                     : laudrup@stacktrace.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CRDATASTREAM_H
#define CRDATASTREAM_H

#include <cassert>

class CrInputDataStream;
class CrOutputDataStream;

/**
	*@author Jozef Kosoru
	*/
class CrDataStream {
public:
	CrDataStream(char* memory, const int length, char* pos) : m_memory(memory), m_length(length), m_pos(pos) {}
	virtual ~CrDataStream() {}
	virtual bool eof() const = 0;
protected:
	friend class CrInputDataStream;
	friend class CrOutputDataStream;
	char* m_memory;
	int m_length;
	char* m_pos;
};

/**
	*@author Jozef Kosoru
	*/
class CrInputDataStream : public CrDataStream {
public:
	CrInputDataStream(char* memory, const int length) : CrDataStream(memory, length, memory) {}
	CrInputDataStream(CrDataStream& stream) : CrDataStream(stream.m_memory, stream.m_length, stream.m_pos) {}
	virtual ~CrInputDataStream() {}
	virtual bool eof() const { return m_pos == (m_memory+m_length); }

	template<class Type> CrInputDataStream& operator<<(const Type& var){
		assert( (m_pos + sizeof(Type)) <= (m_memory + m_length));

		*(reinterpret_cast<Type*>(m_pos)) = var;
		m_pos += sizeof(Type);
		return *this;
	}
};

/**
	*@author Jozef Kosoru
	*/
class CrOutputDataStream : public CrDataStream {
public:
	CrOutputDataStream(char* memory, const int length) : CrDataStream(memory, length, memory) {}
	CrOutputDataStream(CrDataStream& stream) : CrDataStream(stream.m_memory, stream.m_length, stream.m_pos) {}
	virtual ~CrOutputDataStream() {}
	virtual bool eof() const { return m_pos == (m_memory+m_length); }

	template<class Type> CrOutputDataStream& operator>>(Type& var){
		assert( (m_pos + sizeof(Type)) <= (m_memory + m_length));

		var = *(reinterpret_cast<Type*>(m_pos));
		m_pos += sizeof(Type);
		return *this;
	}
};

#endif
