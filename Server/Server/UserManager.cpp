#include "UserManager.h"

UserManager* UserManager::s_pInst = nullptr;

User* UserManager::Create(const wchar_t* _pNickname)
{
	m_lock.Enter();
	User* pUser = Find(_pNickname);
	if (!pUser)
	{
		pUser = new User(_pNickname);
		m_mapUser.insert({ pUser->GetNickname(), pUser });
	}
	m_lock.Leave();

	return pUser;
}

User* UserManager::Find(const wchar_t* _pNickname)
{
	std::unordered_map<std::wstring, User*>::const_iterator iter = m_mapUser.find(_pNickname);
	if (iter == m_mapUser.cend()) return nullptr;

	return iter->second;
}
/*
User* UserManager::FindConnectedUser(uint32 _connectionId)
{
	if (_connectionId == USER_NOT_CONNECTED) return nullptr;

	std::unordered_map<uint32, User*>::const_iterator iter = m_mapConnectedUser.find(_connectionId);
	if (iter == m_mapConnectedUser.cend()) return nullptr;

	return iter->second;
}

void UserManager::Connect(uint32 _connectionId, User* _pUser)
{
	if (FindConnectedUser(_connectionId)) return;
	m_mapConnectedUser.insert({ _connectionId, _pUser });
	_pUser->SetConnectionId(_connectionId);
	_pUser->SetState(eLoginState::Login);
}

void UserManager::Disconnect(uint32 _connectionId)
{
	User* pUser = FindConnectedUser(_connectionId);
	if (!pUser) return;

	m_mapConnectedUser.erase(_connectionId);
	pUser->SetConnectionId(USER_NOT_CONNECTED);
	pUser->SetState(eLoginState::Logout);
}
*/
UserManager::UserManager() :
	m_lock(false)
{
}

UserManager::~UserManager()
{
}
