#include "ConnectionManager.h"
#include "Connection.h"

ConnectionManager* ConnectionManager::s_pInst = nullptr;

Connection* ConnectionManager::Create(SOCKET _socket)
{
	Connection* pConn = new Connection(m_connectionId, _socket);
	m_mapConnection.insert({ m_connectionId, pConn });
	++m_connectionId;
	return pConn;
}

bool ConnectionManager::Delete(uint32 _id)
{
	std::unordered_map<uint32, Connection*>::iterator iter = m_mapConnection.find(_id);
	if (iter == m_mapConnection.end()) return false;

	delete iter->second;
	m_mapConnection.erase(_id);
	return true;
}

ConnectionManager::ConnectionManager() :
	m_connectionId(1)
{
}

ConnectionManager::~ConnectionManager()
{
}
