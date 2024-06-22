#include "DBConnectionManager.h"

DBConnectionManager* DBConnectionManager::s_pInst = nullptr;

DBConnectionManager::DBConnectionManager() :
	m_environment(SQL_NULL_HANDLE), m_userDBConn(nullptr)
{
}

DBConnectionManager::~DBConnectionManager()
{
	Clear();
}

bool DBConnectionManager::Connect(const wchar_t* _pConnStr)
{
	if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, OUT &m_environment) != SQL_SUCCESS) return false;
	if (::SQLSetEnvAttr(m_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS) return false;

	m_userDBConn = new DBConnection();
	if (m_userDBConn->Connect(m_environment, _pConnStr) == false) return false;

	return true;
}

void DBConnectionManager::Clear()
{
	if (m_environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_environment);
		m_environment = SQL_NULL_HANDLE;
	}

	m_userDBConn->Clear();
	delete m_userDBConn;
	m_userDBConn = nullptr;
}

DBConnection* DBConnectionManager::Acquire()
{
	m_lock.Enter();

	return m_userDBConn;
}

void DBConnectionManager::Release()
{

	m_lock.Leave();
}
