#pragma once

#include "DBConnection.h"
#include "../Lock/CSLock.h"

class DBConnectionManager
{
private:
	CSLock m_lock;

private:
	SQLHENV								m_environment;
	DBConnection*						m_userDBConn;

public:
	bool Connect(const wchar_t* _pConnStr);
	void Clear();

	DBConnection* Acquire();
	void Release();

#pragma region Singleton
private:
	static DBConnectionManager* s_pInst;

public:
	static DBConnectionManager* GetInst()
	{
		if (!s_pInst)
			s_pInst = new DBConnectionManager;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	DBConnectionManager();
	~DBConnectionManager();
#pragma endregion 
};

