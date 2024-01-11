#include "User.h"

User::User(const wchar_t* _pNickname) : 
	m_eState(eLoginState::Logout),
	/* m_connectionId(USER_NOT_CONNECTED), */
	m_loginCount(0), 
	m_killCount(0)
	//, m_lock(true)
{
	uint32 len = (uint32)wcslen(_pNickname);
	if (len > NICKNAME_LEN)			len = NICKNAME_LEN;
	wcsncpy_s(m_nickname, _pNickname, len);
}

User::~User()
{
}