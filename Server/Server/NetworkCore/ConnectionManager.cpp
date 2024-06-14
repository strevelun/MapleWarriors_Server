#include "ConnectionManager.h"
#include "../User/UserManager.h"

ConnectionManager* ConnectionManager::s_pInst = nullptr;

std::shared_ptr<Connection> ConnectionManager::Create(tAcceptedClient* _pAcceptedClient)
{
	std::shared_ptr<Connection> conn = std::make_shared<Connection>(m_connectionId, _pAcceptedClient);
	m_mapConnection.insert({ m_connectionId, conn });
	++m_connectionId;
	++m_count;
	return conn;
}

std::shared_ptr<Connection> ConnectionManager::Get(uint32 _id)
{
	m_srwLock.Lock(eLockType::Reader);
	std::unordered_map<uint32, std::shared_ptr<Connection>>::iterator iter = m_mapConnection.find(_id);
	if (iter == m_mapConnection.cend())
	{
		m_srwLock.UnLock(eLockType::Reader);
		return nullptr;
	}
	m_srwLock.UnLock(eLockType::Reader);
	return iter->second;
}

void ConnectionManager::Delete(uint32 _id)
{
	m_srwLock.Lock(eLockType::Writer);
	std::unordered_map<uint32, std::shared_ptr<Connection>>::iterator iter = m_mapConnection.find(_id);
	if (iter != m_mapConnection.cend())
	{
		--m_count;
		printf("id[%u], socket[%d], IP[%s]		접속 종료됨		(현재 접속자 수 : %d)\n", _id, static_cast<int32>(iter->second->GetSocket()), iter->second->GetIP(), m_count);
		iter->second = nullptr;
		m_mapConnection.erase(_id);
	}
	m_srwLock.UnLock(eLockType::Writer);
}

ConnectionManager::ConnectionManager() :
	m_connectionId(1), m_count(0)
{}

ConnectionManager::~ConnectionManager()
{}