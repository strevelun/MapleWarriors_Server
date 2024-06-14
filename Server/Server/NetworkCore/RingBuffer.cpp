#include "RingBuffer.h"


RingBuffer::RingBuffer() :
	m_buffer{}, m_readPos(0), m_writePos(0), m_writtenBytes(0)
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
	if (IsFull()) return BUFFER_MAX - m_readPos;

	return (m_readPos <= m_writePos) ? m_writePos - m_readPos : BUFFER_MAX - m_readPos;
}

uint32 RingBuffer::GetTotalReadableSize() const
{
	if (IsFull()) return BUFFER_MAX;

	return (m_writePos < m_readPos) ? BUFFER_MAX - m_readPos + m_writePos : m_writePos - m_readPos; ;
}

void RingBuffer::SetWriteBuf(OUT WSABUF& _buf)
{
	uint32 writableSize = GetWritableSize();

	_buf.buf = &m_buffer[m_writePos];
	_buf.len = writableSize;
}

void RingBuffer::MoveReadPos(uint32 _readBytes)
{
	m_writtenBytes -= _readBytes;
	m_readPos = (_readBytes + m_readPos) % BUFFER_MAX;
}

void RingBuffer::MoveWritePos(uint32 _recvBytes)
{
#ifdef _DEBUG
	m_totalRecvBytes += _recvBytes;
#endif

	m_writtenBytes += _recvBytes;
	m_writePos = (_recvBytes + m_writePos) % BUFFER_MAX;
	//printf("m_readPos : %d, m_writePos : %d\n", m_readPos, m_writePos);
}
/*
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
*/