#pragma once

#include "User.h"
#include "../Lock/SRWLock.h"

class UserManager
{
private:
	SRWLock		m_lock;

private:
	std::unordered_map<uint32, User*>			m_umapConnectedUser;

public:
	User* Create(uint32 _connID, const wchar_t* _pNickname);
	User* FindConnectedUser(uint32 _connID);
	void Disconnect(uint32 _connID);

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

