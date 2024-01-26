#include "UserManager.h"

UserManager* UserManager::s_pInst = nullptr;

User* UserManager::Create(const wchar_t* _pNickname)
{
	User* pUser = nullptr;
	m_lock.Enter();
	std::unordered_map<std::wstring, User*>::const_iterator iter = m_mapUser.find(_pNickname);
	if (iter == m_mapUser.cend())
	{
		pUser = new User(_pNickname);
		m_mapUser.insert({ _pNickname, pUser });
	}
	else
		pUser = iter->second;
	m_lock.Leave();

	return pUser;
}

User* UserManager::Find(const wchar_t* _pNickname)
{
	User* pUser = nullptr;
	m_lock.Enter();
	std::unordered_map<std::wstring, User*>::const_iterator iter = m_mapUser.find(_pNickname);
	if (iter != m_mapUser.cend()) pUser = iter->second;
	m_lock.Leave();

	return pUser;
}

User* UserManager::FindConnectedUser(uint32 _connectionId)
{
	if (_connectionId == USER_NOT_CONNECTED) return nullptr;

	User* pUser = nullptr;
	m_lock.Enter();
	std::unordered_map<uint32, User*>::const_iterator iter = m_mapConnectedUser.find(_connectionId);
	if (iter != m_mapConnectedUser.cend()) pUser = iter->second;
	m_lock.Leave();

	return pUser;
}

bool UserManager::Connect(uint32 _connectionId, User* _pUser)
{
	m_lock.Enter();
	std::unordered_map<uint32, User*>::const_iterator iter = m_mapConnectedUser.find(_connectionId);
	if (iter == m_mapConnectedUser.cend())
	{
		m_mapConnectedUser.insert({ _connectionId, _pUser });
		_pUser->SetConnectionId(_connectionId);
		_pUser->SetState(eLoginState::Login);
	}
	m_lock.Leave();

	return true;
}

void UserManager::Disconnect(uint32 _connectionId)
{
	m_lock.Enter();
	std::unordered_map<uint32, User*>::const_iterator iter = m_mapConnectedUser.find(_connectionId);
	if (iter != m_mapConnectedUser.cend())
	{
		User* pUser = iter->second;
		m_mapConnectedUser.erase(_connectionId);
		pUser->SetConnectionId(USER_NOT_CONNECTED);
		pUser->SetState(eLoginState::Logout);
	}
	m_lock.Leave();
}

UserManager::UserManager() :
	m_lock(false)
{
}

UserManager::~UserManager()
{
}
