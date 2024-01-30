#pragma once

#include "../Defines.h"
#include "../Packet/Types/PacketTypes.h"

class RingBuffer
{
private:
#ifdef _DEBUG
	uint32 m_totalRecvBytes = 0;
#endif
	char m_buffer[BUFFER_MAX];
	uint32 m_readPos;
	uint32 m_writePos;
	uint32 m_writtenBytes;

public:
	RingBuffer();
	~RingBuffer();

	const char* GetBuffer() const { return m_buffer; }
	char* GetReadAddr() { return &m_buffer[m_readPos]; }
	char* GetWriteAddr() { return &m_buffer[m_writePos]; }
	uint32 GetWrittenBytes() const { return m_writtenBytes;	}
	uint32 GetWritableSize() const;
	uint32 GetReadableSize() const;
	uint32 GetTotalReadableSize() const;
	uint32 GetReadPos() { return m_readPos; }
	
	bool IsFull() const { return m_writtenBytes >= BUFFER_MAX; }

	bool SetWriteBuf(WSABUF& _buf);

	void MoveReadPos(uint32 _readBytes);
	void MoveWritePos(uint32 _recvBytes);

	//void HandleVerge();
};

