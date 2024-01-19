#include "RingBuffer.h"


RingBuffer::RingBuffer() :
	m_buffer{}, m_tempBuffer{}, m_readPos(0), m_writePos(0), m_tempPos(0), m_bIsTempUsed(false), m_writtenBytes(0)
{
}

RingBuffer::~RingBuffer()
{
}

uint32 RingBuffer::GetWritableSize() const
{
	if (IsFull()) return 0;

	return (m_readPos <= m_writePos) ? BUFFER_MAX - m_writePos : m_readPos - m_writePos;
}

uint32 RingBuffer::GetReadableSize() const
{
	//if (IsFull()) return BUFFER_MAX;
	if (m_bIsTempUsed) return m_tempPos;

	return (m_readPos <= m_writePos) ? m_writePos - m_readPos : BUFFER_MAX - m_readPos;
}

bool RingBuffer::SetWriteBuf(WSABUF& _buf)
{
	uint32 writableSize = GetWritableSize();
	if (writableSize == 0) return false;

	if (m_bIsTempUsed)
	{
		_buf.buf = &m_tempBuffer[m_tempPos];
		uint16 size = m_tempPos < sizeof(PacketSize) ? PACKET_HEADER_SIZE : *reinterpret_cast<const uint16*>(m_tempBuffer);
		_buf.len = size - m_tempPos;
	}
	else 
	{
		_buf.buf = &m_buffer[m_writePos];
		_buf.len = writableSize;
		printf("writePos : %d, writableSize : %d\n", m_writePos, writableSize);
	}
	return true;
}

void RingBuffer::MoveReadPos(uint32 _readBytes)
{
	m_writtenBytes -= _readBytes;
	m_readPos = (_readBytes + m_readPos) % BUFFER_MAX;
	if (m_bIsTempUsed)
	{
		m_bIsTempUsed = false;
		m_tempPos = 0;
	}
}

void RingBuffer::MoveWritePos(uint32 _recvBytes)
{
	m_totalRecvBytes += _recvBytes;

	m_writtenBytes += _recvBytes;
	m_writePos = (_recvBytes + m_writePos) % BUFFER_MAX;
	//printf("m_readPos : %d, m_writePos : %d\n", m_readPos, m_writePos);
	if (m_bIsTempUsed) m_tempPos += _recvBytes;
}

void RingBuffer::HandleVerge()
{
	if (m_bIsTempUsed) return;

	uint32 readableSize = GetReadableSize();
	uint16 cpySize = 0;
	if(readableSize >= sizeof(PacketSize))
		cpySize = *reinterpret_cast<const uint16*>(&m_buffer[m_readPos]);

	if (readableSize == 1 || m_readPos + cpySize > BUFFER_MAX)
	{
		m_tempPos = BUFFER_MAX - m_readPos;
		memcpy(m_tempBuffer, &m_buffer[m_readPos], m_tempPos);
		m_bIsTempUsed = true;
		printf("TEMPBUFFER\n");
	}
	//printf("HANDLEVERGE : %d, %d\n", m_readPos, m_writePos);
}
