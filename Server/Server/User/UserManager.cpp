#include "UserManager.h"
#include "../NetworkCore/ConnectionManager.h"

UserManager* UserManager::s_pInst = nullptr;

User* UserManager::Create(const wchar_t* _pNickname)
{
	User* pUser = nullptr;
	m_userDBLock.Enter();	
	std::unordered_map<std::wstring, User*>::iterator iter = m_umapDBUser.find(_pNickname);
	std::unordered_map<std::wstring, User*>::iterator iterEnd = m_umapDBUser.end();
	if (iter == iterEnd)
	{
		pUser = new User(_pNickname);
		m_umapDBUser.insert({ _pNickname, pUser });
	}
	else
		pUser = iter->second;
	m_userDBLock.Leave();

	return pUser;
}

User* UserManager::FindConnectedUser(uint32 _connectionId)
{
	if (_connectionId == USER_NOT_CONNECTED) return nullptr;

	User* pUser = nullptr;

	m_lock.Lock(eLockType::Reader);
	std::unordered_map<uint32, User*>::iterator iter = m_umapConnectedUser.find(_connectionId);
	if (iter != m_umapConnectedUser.end()) pUser = iter->second;
	m_lock.UnLock(eLockType::Reader);

	return pUser;
}

// 유저만들고 insert
bool UserManager::Connect(uint32 _connectionId, const wchar_t* _pNickname)
{
	m_lock.Lock(eLockType::Writer);
	std::unordered_map<std::wstring, User*>::iterator iter = m_umapDBUser.find(_pNickname);
	std::unordered_map<std::wstring, User*>::iterator iterEnd = m_umapDBUser.end();
	if (iter == iterEnd)
	{
		m_lock.UnLock(eLockType::Writer);
		return false;
	}
	User* pUser = iter->second;
	m_umapConnectedUser.insert({ _connectionId, pUser });
	m_lock.UnLock(eLockType::Writer);

	pUser->Connect(_connectionId);

	return true;
}

void UserManager::Disconnect(uint32 _connectionId)
{
	User* pUser = nullptr;

	m_lock.Lock(eLockType::Writer);
	std::unordered_map<uint32, User*>::iterator iter = m_umapConnectedUser.find(_connectionId);
	if (iter != m_umapConnectedUser.end()) pUser = iter->second;
	if (!pUser)
	{
		m_lock.UnLock(eLockType::Writer);
		return;
	}

	m_umapConnectedUser.erase(_connectionId);
	m_lock.UnLock(eLockType::Writer);

	pUser->Leave();
}

UserManager::UserManager() 
{
}

UserManager::~UserManager()
{
}
