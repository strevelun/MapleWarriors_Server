#include "PacketReader.h"

PacketReader::PacketReader() :
	m_pBuffer(nullptr), m_getPos(2), m_startOffset(0)
{
}

PacketReader::~PacketReader()
{
}

void PacketReader::SetBuffer(RingBuffer& _buffer)
{
	m_pBuffer = _buffer.GetBuffer();
	uint32 pos = _buffer.GetReadPos();
	m_getPos = (pos + sizeof(PacketSize)) % BUFFER_MAX;
	m_startOffset = pos;
}

bool PacketReader::IsBufferReadable(RingBuffer& _buffer)
{
	uint32 totalReadableSize = _buffer.GetTotalReadableSize();
	if (totalReadableSize < sizeof(PacketSize))			return false;

	uint16 packetSize;
	uint32 readPos = _buffer.GetReadPos();
	const int8* pBuf = _buffer.GetBuffer();
	if (readPos + sizeof(PacketSize) > BUFFER_MAX)
	{
		int8 first = pBuf[readPos];
		int8 second = pBuf[0];
		packetSize = (second << 8) | first;
	}
	else
		packetSize = *reinterpret_cast<const uint16*>(&pBuf[readPos]);

	if (packetSize > totalReadableSize)	return false;

	return true;
}

uint32 PacketReader::GetSize() const
{
	if (!m_pBuffer) return 0;

	int32 size = 0;
	if (m_startOffset + sizeof(PacketSize) > BUFFER_MAX)
	{
		int8 first = m_pBuffer[m_startOffset];
		int8 second = m_pBuffer[0];
		size = (second << 8) | first;
	}
	else
		size = *reinterpret_cast<const uint16*>(&m_pBuffer[m_startOffset]);
	return size;
}

const wchar_t* PacketReader::GetWString()
{
	uint16 pos = m_getPos;
	const wchar_t* str = reinterpret_cast<const wchar_t*>(&m_pBuffer[pos]);
	uint32 maxCnt = (BUFFER_MAX - pos) / 2;
	uint32 len = wcsnlen_s(str, maxCnt);
	if (len < maxCnt) // 정상적인 경우
	{
		m_getPos += int32(len * sizeof(wchar_t) + sizeof(wchar_t));
		return str;
	}
	else
	{
		uint32 tempPos = BUFFER_MAX - pos;
		memcpy(m_tempBuf, &m_pBuffer[pos], tempPos);
		if (tempPos % 2 != 0) // 2바이트씩 읽는데 1바이트가 끄트머리에 남은 경우
		{
			m_tempBuf[tempPos++] = m_pBuffer[0];
			str = reinterpret_cast<const wchar_t*>(&m_pBuffer[1]);
			uint32 cpySize = wcslen(str) * sizeof(wchar_t) + sizeof(wchar_t);
			memcpy(&m_tempBuf[tempPos], &m_pBuffer[1], cpySize);
			m_getPos += tempPos + cpySize;
		}
		else
		{
			str = reinterpret_cast<const wchar_t*>(m_pBuffer);
			uint32 cpySize = wcslen(str) * sizeof(wchar_t) + sizeof(wchar_t);
			memcpy(&m_tempBuf[tempPos], m_pBuffer, cpySize);
			m_getPos += tempPos + cpySize;
		}
		return reinterpret_cast<const wchar_t*>(m_tempBuf);
	}
}

PacketType PacketReader::GetPacketType()
{
	return GetUShort();
}

int8 PacketReader::GetInt8()
{
	int32 pos = m_getPos;
	m_getPos = (sizeof(int8) + m_getPos) % BUFFER_MAX;
	return m_pBuffer[pos];
}

uint16 PacketReader::GetShort()
{
	int32 pos = m_getPos;
	m_getPos = (sizeof(int16) + m_getPos) % BUFFER_MAX;
	int16 result;
	if (pos + sizeof(int16) > BUFFER_MAX)
	{
		int8 first = m_pBuffer[pos];
		int8 second = m_pBuffer[0];
		result = (second << 8) | first;
	}
	else
		result = *reinterpret_cast<const int16*>(&m_pBuffer[pos]);
	return result;
}

uint16 PacketReader::GetUShort()
{
	int32 pos = m_getPos;
	m_getPos = (sizeof(uint16) + m_getPos) % BUFFER_MAX;
	uint16 result;
	if (pos + sizeof(uint16) > BUFFER_MAX)
	{
		int8 first = m_pBuffer[pos];
		int8 second = m_pBuffer[0];
		result = (second << 8) | first;
	}
	else
		result = *reinterpret_cast<const uint16*>(&m_pBuffer[pos]);
	return result;
}

int32 PacketReader::GetInt32()
{
	int32 pos = m_getPos;
	m_getPos = (sizeof(int32) + m_getPos) % BUFFER_MAX;
	int32 result = 0;
	if (pos + sizeof(int32) > BUFFER_MAX)
	{
		int32 i = pos;
		int32 bit = 0;
		int8 temp;
		while (bit < sizeof(int32))
		{
			temp = m_pBuffer[i];
			result |= temp << (bit * 8);
			++bit;
			i = (i + 1) % BUFFER_MAX;
		}
	}
	else
		result = *reinterpret_cast<const int32*>(&m_pBuffer[pos]);
	return result;
}

uint32 PacketReader::GetUInt32()
{
	uint32 pos = m_getPos;
	m_getPos = (sizeof(uint32) + m_getPos) % BUFFER_MAX;
	uint32 result = 0;
	if (pos + sizeof(uint32) > BUFFER_MAX)
	{
		uint32 i = pos;
		uint32 bit = 0;
		int8 temp;
		while (bit < sizeof(uint32))
		{
			temp = m_pBuffer[i];
			result |= temp << (bit * 8);
			++bit;
			i = (i + 1) % BUFFER_MAX;
		}
	}
	else
		result = *reinterpret_cast<const uint32*>(&m_pBuffer[pos]);
	return result;
}

int64 PacketReader::GetInt64()
{
	int64 pos = m_getPos;
	m_getPos = (sizeof(int64) + m_getPos) % BUFFER_MAX;
	int64 result = 0;
	if (pos + sizeof(int64) > BUFFER_MAX)
	{
		int64 i = pos;
		int64 bit = 0;
		int8 temp;
		while (bit < sizeof(int64))
		{
			temp = m_pBuffer[i];
			result |= temp << (bit * 8);
			++bit;
			i = (i + 1) % BUFFER_MAX;
		}
	}
	else
		result = *reinterpret_cast<const int64*>(&m_pBuffer[pos]);
	return result;
}
