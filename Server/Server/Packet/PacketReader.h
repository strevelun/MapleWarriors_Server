#pragma once

#include "../NetworkCore/RingBuffer.h"

class PacketReader
{
private:
	const char*		m_pBuffer;
	char			m_tempBuf[PACKET_MAX_SIZE];
	uint16			m_getPos;
	uint16			m_startOffset;

public:
	PacketReader();
	~PacketReader();

	void SetBuffer(RingBuffer& _buffer);

	bool IsBufferReadable(RingBuffer& _buffer);

	uint32 GetSize() const;

	//template <typename Type>
	//Type Get();
	PacketType GetPacketType();
	char GetChar();
	uint16 GetUShort();
	const wchar_t* GetWString();

};
/*
template<typename Type>
inline Type PacketReader::Get()
{
	if (std::is_same<Type, char>::value)
	{
		GetChar();
	}
	uint16 pos = m_getPos;
	m_getPos += sizeof(Type);
	return *reinterpret_cast<Type*>(&m_pBuffer[pos]);
}
*/