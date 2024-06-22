#include "UserManager.h"
#include "../NetworkCore/ConnectionManager.h"

UserManager* UserManager::s_pInst = nullptr;

User* UserManager::Create(uint32 _connID, const wchar_t* _pNickname)
{
	User* pUser = nullptr;
	m_lock.Lock(eLockType::Writer);
	std::unordered_map<uint32, User*>::iterator iter = m_umapConnectedUser.find(_connID);
	std::unordered_map<uint32, User*>::iterator iterEnd = m_umapConnectedUser.end();
	if (iter == iterEnd)
	{
		pUser = new User(_connID, _pNickname);
		m_umapConnectedUser.insert({ _connID, pUser });
	}
	else
		pUser = iter->second;
	m_lock.UnLock(eLockType::Writer);

	return pUser;
}

User* UserManager::FindConnectedUser(uint32 _connID)
{
	if (_connID == USER_NOT_CONNECTED) return nullptr;

	User* pUser = nullptr;

	m_lock.Lock(eLockType::Reader);
	std::unordered_map<uint32, User*>::iterator iter = m_umapConnectedUser.find(_connID);
	if (iter != m_umapConnectedUser.end()) pUser = iter->second;
	m_lock.UnLock(eLockType::Reader);

	return pUser;
}

void UserManager::Disconnect(uint32 _connID)
{
	User* pUser = nullptr;

	m_lock.Lock(eLockType::Writer);
	std::unordered_map<uint32, User*>::iterator iter = m_umapConnectedUser.find(_connID);
	if (iter != m_umapConnectedUser.end()) pUser = iter->second;
	if (!pUser)
	{
		m_lock.UnLock(eLockType::Writer);
		return;
	}

	m_umapConnectedUser.erase(_connID);
	m_lock.UnLock(eLockType::Writer);

	pUser->Leave(); // 로비, 룸에서 User nullptr로 세팅 

	delete pUser;
}

UserManager::UserManager() 
{
}

UserManager::~UserManager()
{
}
