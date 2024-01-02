#pragma once

#include "../NetworkCore/RingBuffer.h"

class PacketReader
{
private:
	char* m_pBuffer;
	uint16		m_getPos;

public:
	PacketReader();
	~PacketReader();

	void SetBuffer(RingBuffer& _buffer);

	bool IsBufferReadable(RingBuffer& _buffer);

	uint32 GetSize() const;

	template <typename Type>
	Type Get();
	const wchar_t* GetWString();
};

template<typename Type>
inline Type PacketReader::Get()
{
	uint16 pos = m_getPos;
	m_getPos += sizeof(Type);
	return *reinterpret_cast<Type*>(&m_pBuffer[pos]);
}