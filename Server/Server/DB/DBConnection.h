#pragma once

#include "../Defines.h"

#include <sql.h>
#include <sqlext.h>

class DBConnection
{
private:


private:
	SQLHDBC			m_conn = SQL_NULL_HANDLE;
	SQLHSTMT		m_statement = SQL_NULL_HANDLE;

public:
	bool Connect(SQLHDBC _henv, const wchar_t* _pConnStr);
	void Clear();

	bool Execute(const wchar_t* _pQuery);
	bool Fetch();
	int32 GetRowCount();
	void Unbind();

	bool BindParam(SQLUSMALLINT _paramIdx, SQLSMALLINT _cType, SQLSMALLINT sqlType, SQLULEN _len, SQLPOINTER _pPtr, SQLLEN* _pIndex); // 실행할 때 주는 파라미터
	bool BindCol(SQLUSMALLINT _columnIndex, SQLSMALLINT _cType, SQLULEN _len, SQLPOINTER _value, SQLLEN* _pIndex);
	void HandleError(SQLRETURN _ret);
};

