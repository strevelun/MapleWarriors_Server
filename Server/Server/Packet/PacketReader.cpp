#include "PacketReader.h"

PacketReader::PacketReader() :
	m_pBuffer(nullptr), m_getPos(sizeof(PacketSize))
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

uint32 PacketReader::GetPacketSize() const
{
	if (!m_pBuffer) return 0;
	return *reinterpret_cast<const uint16*>(m_pBuffer);
}
