#include "DBConnection.h"

bool DBConnection::Connect(SQLHDBC _henv, const wchar_t* _pConnStr)
{
    if (::SQLAllocHandle(SQL_HANDLE_DBC, _henv, &m_conn) != SQL_SUCCESS) return false;

    wchar_t strBuffer[MAX_PATH] = { 0 };
    wcscpy_s(strBuffer, _pConnStr);

    wchar_t resultStr[MAX_PATH] = { 0 };
    SQLSMALLINT resultStringLen = 0;

    SQLRETURN ret = ::SQLDriverConnectW(
        m_conn, NULL, reinterpret_cast<SQLWCHAR*>(strBuffer),
        _countof(strBuffer),
        OUT reinterpret_cast<SQLWCHAR*>(resultStr),
        _countof(resultStr),
        OUT &resultStringLen,
        SQL_DRIVER_NOPROMPT
    );

    if (::SQLAllocHandle(SQL_HANDLE_STMT, m_conn, &m_statement) != SQL_SUCCESS) return false;

    return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

void DBConnection::Clear()
{
    if (m_conn != SQL_NULL_HANDLE)
    {
        ::SQLFreeHandle(SQL_HANDLE_DBC, m_conn);
        m_conn = SQL_NULL_HANDLE;
    }
    if (m_statement != SQL_NULL_HANDLE)
    {
        ::SQLFreeHandle(SQL_HANDLE_STMT, m_statement);
        m_statement = SQL_NULL_HANDLE;
    }
}

bool DBConnection::Execute(const wchar_t* _pQuery)
{
    SQLRETURN ret = ::SQLExecDirectW(m_statement, (SQLWCHAR*)_pQuery, SQL_NTSL);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) return true;

    HandleError(ret);

    return false;
}

bool DBConnection::Fetch()
{
    SQLRETURN ret = ::SQLFetch(m_statement);

    switch (ret)
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        return true;
    case SQL_NO_DATA:
        return false;
    case SQL_ERROR:
        HandleError(ret);
        return false;
    default:
        return true;
    }

    return false;
}

int32 DBConnection::GetRowCount()
{
    SQLLEN count = 0;
    SQLRETURN ret = ::SQLRowCount(m_statement, OUT &count);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) return static_cast<int32>(count);

    return -1;
}

void DBConnection::Unbind()
{
    ::SQLFreeStmt(m_statement, SQL_UNBIND);
    ::SQLFreeStmt(m_statement, SQL_RESET_PARAMS);
    ::SQLFreeStmt(m_statement, SQL_CLOSE);
}

bool DBConnection::BindParam(SQLUSMALLINT _paramIdx, SQLSMALLINT _cType, SQLSMALLINT sqlType, SQLULEN _len, SQLPOINTER _pPtr, SQLLEN* _pIndex) 
{
    SQLRETURN ret = ::SQLBindParameter(m_statement, _paramIdx, SQL_PARAM_INPUT, _cType, sqlType, _len, 0, _pPtr, 0, _pIndex);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        HandleError(ret);
        return false;
    }

    return true;
}

bool DBConnection::BindCol(SQLUSMALLINT _columnIndex, SQLSMALLINT _cType, SQLULEN _len, SQLPOINTER _value, SQLLEN* _pIndex)
{
    SQLRETURN ret = ::SQLBindCol(m_statement, _columnIndex, _cType, _value, _len, _pIndex);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        HandleError(ret);
        return false;
    }
    return true;
}

void DBConnection::HandleError(SQLRETURN _ret)
{
    if (_ret == SQL_SUCCESS) return;

    SQLSMALLINT index = 1;
    SQLWCHAR sqlState[MAX_PATH] = { 0 };
    SQLINTEGER nativeErr = 0;
    SQLWCHAR errMsg[MAX_PATH] = { 0 };
    SQLSMALLINT msgLen = 0;
    SQLRETURN errorRet = 0;

    while (true)
    {
        ::SQLGetDiagRecW(
            SQL_HANDLE_STMT,
            m_statement,
            index,
            sqlState,
            OUT &nativeErr,
            errMsg,
            _countof(errMsg),
            OUT &msgLen
        );

        if (errorRet == SQL_NO_DATA) break;
        if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO) break;

        wprintf(L"DBError : %s\n", errMsg);

        ++index;
    }
}
