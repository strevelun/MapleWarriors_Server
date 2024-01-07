#include "PacketReader.h"

PacketReader::PacketReader() :
	m_pBuffer(nullptr), m_getPos(2)
{
}

PacketReader::~PacketReader()
{
}

void PacketReader::SetBuffer(RingBuffer& _buffer)
{
	m_pBuffer = _buffer.GetReadAddr();
	m_getPos = sizeof(PacketSize);
}

bool PacketReader::IsBufferReadable(RingBuffer& _buffer)
{
	uint32 readableSize = _buffer.GetReadableSize();
	if (readableSize < sizeof(PacketSize))			return false;

	uint16 packetSize = *reinterpret_cast<const uint16*>(_buffer.GetReadAddr());
	if (packetSize > readableSize)	return false;

	return true;
}

uint32 PacketReader::GetSize() const
{
	if (!m_pBuffer) return 0;
	return *reinterpret_cast<const uint16*>(m_pBuffer);
}

const wchar_t* PacketReader::GetWString()
{
	uint16 pos = m_getPos;
	const wchar_t* str = reinterpret_cast<const wchar_t*>(&m_pBuffer[pos]);
	m_getPos += int(wcslen(str) * sizeof(wchar_t) + sizeof(wchar_t));
	return str;
}
