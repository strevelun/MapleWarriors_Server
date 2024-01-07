#pragma once

#include "Defines.h"
#include "Types.h"
#include "NetworkCore/Connection.h"
#include "CSLock.h"

class User
{
private:
	//CSLock		m_lock;

private:
	eLoginState		m_eState; // ?
	//uint32			m_connectionId;
	wchar_t			m_nickname[NICKNAME_LEN];
	uint32			m_loginCount;
	uint32			m_killCount;

public:
	User(const wchar_t* _pNickname);
	~User();

	const wchar_t* GetNickname() const { return m_nickname; }
	eLoginState GetState() const { return m_eState; }

	//void SetConnectionId(uint32 _connectionId) { m_connectionId = _connectionId; }
	void SetState(eLoginState _eState) { m_eState = _eState; }

	void HandleLogin(Connection& _conn);
};

