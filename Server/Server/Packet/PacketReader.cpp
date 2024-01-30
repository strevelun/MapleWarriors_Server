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
	const char* pBuf = _buffer.GetBuffer();
	if (readPos + sizeof(PacketSize) > BUFFER_MAX)
	{
		char first = pBuf[readPos];
		char second = pBuf[0];
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

	int size = 0;
	if (m_startOffset + sizeof(PacketSize) > BUFFER_MAX)
	{
		byte first = m_pBuffer[m_startOffset];
		byte second = m_pBuffer[0];
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
		m_getPos += int(len * sizeof(wchar_t) + sizeof(wchar_t));
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

char PacketReader::GetChar()
{
	int pos = m_getPos;
	m_getPos = (sizeof(char) + m_getPos) % BUFFER_MAX;
	return m_pBuffer[pos];
}

uint16 PacketReader::GetUShort()
{
	int pos = m_getPos;
	m_getPos = (sizeof(uint16) + m_getPos) % BUFFER_MAX;
	uint16 result;
	if (pos + sizeof(uint16) > BUFFER_MAX)
	{
		byte first = m_pBuffer[pos];
		byte second = m_pBuffer[0];
		result = (second << 8) | first;
	}
	else
		result = *reinterpret_cast<const uint16*>(&m_pBuffer[pos]);
	return result;
}
