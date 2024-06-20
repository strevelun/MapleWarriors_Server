#pragma once

#include "../NetworkCore/RingBuffer.h"

class PacketReader
{
private:
	const int8*		m_pBuffer;
	int8			m_tempBuf[PACKET_MAX_SIZE];
	uint16			m_getPos;
	uint16			m_startOffset;

public:
	PacketReader();
	~PacketReader();

	void SetBuffer(const int8* _pBuffer);
	void SetBuffer(RingBuffer& _buffer);

	bool IsBufferReadable(RingBuffer& _buffer);

	uint32 GetSize() const;

	PacketType GetPacketType();
	int8 GetInt8();
	uint8 GetUInt8();
	uint16 GetShort();
	uint16 GetUShort();
	int32 GetInt32();
	uint32 GetUInt32();
	int64 GetInt64();
	const wchar_t* GetWString();

};