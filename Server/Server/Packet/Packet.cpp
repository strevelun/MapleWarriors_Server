#include "Packet.h"

Packet::Packet() :
	m_buffer{}, m_addPos(2)
{
}

Packet::~Packet()
{
}

Packet& Packet::AddWString(const wchar_t* _str)
{
	std::wstring str = _str ? _str : L"";
	uint16 size = (uint16)str.size();
	uint16 byteSize = size * sizeof(wchar_t);
	memcpy(&m_buffer[m_addPos], str.c_str(), byteSize);
	m_addPos += byteSize;
	m_addPos += sizeof(wchar_t);
	*reinterpret_cast<uint16*>(m_buffer) = m_addPos;
	return *this;
}
