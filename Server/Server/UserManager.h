#pragma once

#include <unordered_map>
#include <string>

#include "User.h"
#include "CSLock.h"

class UserManager
{
private:
	CSLock		m_lock;

private:
	std::unordered_map<std::wstring, User*>		m_mapUser;
	//std::unordered_map<uint32, User*>			m_mapConnectedUser;

public:
	User* Create(const wchar_t* _pNickname);
	User* Find(const wchar_t* _pNickname);
	//User* FindConnectedUser(uint32 _connectionId);
	//void Connect(uint32 _connectionId, User* _pUser);
	//void Disconnect(uint32 _connectionId);

#pragma region Singleton
private:
	static UserManager* s_pInst;

public:
	static UserManager* GetInst()
	{
		if (!s_pInst)
			s_pInst = new UserManager;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	UserManager();
	~UserManager();
#pragma endregion 
};

