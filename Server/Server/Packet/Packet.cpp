#include "Packet.h"

Packet::Packet() :
	m_buffer{}, m_addPos(2)
{
}

Packet::~Packet()
{
}

Packet& Packet::AddWString(const std::wstring& _str)
{
	uint16 size = (uint16)_str.size();
	uint16 byteSize = size * sizeof(wchar_t);
	memcpy(&m_buffer[m_addPos], _str.c_str(), byteSize);
	m_addPos += byteSize;
	m_addPos += sizeof(wchar_t);
	*reinterpret_cast<uint16*>(m_buffer) = m_addPos;
	return *this;
}
