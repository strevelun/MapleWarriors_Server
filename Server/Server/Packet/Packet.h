#pragma once

#include "../Types.h"
#include "../Defines.h"

class Packet
{
private:
	char m_buffer[PACKET_MAX_SIZE];
	uint16 m_addPos;

public:
	Packet();
	~Packet();

	const char* GetBuffer() const { return m_buffer; }
	uint16 GetAddedSize() const { return m_addPos; }
	uint16 GetPacketSize() const { return *reinterpret_cast<const uint16*>(m_buffer); }

	template<typename Type>
	Packet& Add(const Type& _type);
};

template<typename Type>
inline Packet& Packet::Add(const Type& _type)
{
	*reinterpret_cast<Type*>(&m_buffer[m_addPos]) = _type;
	m_addPos += sizeof(Type);
	*reinterpret_cast<uint16*>(m_buffer) = m_addPos;
	return *this;
}