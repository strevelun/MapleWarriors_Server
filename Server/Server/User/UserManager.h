#pragma once

#include "User.h"
#include "../SRWLock.h"
#include "../CSLock.h"

class UserManager
{
private:
	SRWLock		m_lock;
	CSLock		m_userDBLock;

private:
	std::unordered_map<std::wstring, User*>		m_mapUser;
	std::unordered_map<uint32, User*>			m_mapConnectedUser;

public:
	User* Create(const wchar_t* _pNickname);
	User* FindConnectedUser(uint32 _connectionId);
	bool Connect(uint32 _connectionId, const wchar_t* _pNickname);
	void Disconnect(uint32 _connectionId);

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

