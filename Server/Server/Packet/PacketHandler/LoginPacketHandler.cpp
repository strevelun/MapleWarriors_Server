#include "LoginPacketHandler.h"
#include "../../User/UserManager.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"
#include "../../DB/DBConnectionManager.h"

// 패킷 지연 시간 테스트
void NLogin::Test(Connection& _conn, PacketReader& _packet)
{
	int64 ticks = _packet.GetInt64();
	Packet pkt;
	pkt.Add<PacketType>(static_cast<PacketType>(eServer::Test));
	//Sleep(20);
	_conn.Send(pkt);
}

void NLogin::LoginReq(Connection& _conn, PacketReader& _packet)
{
	auto start = std::chrono::high_resolution_clock::now();

	const wchar_t* pNickname = _packet.GetWString();

	uint8 ipBytes[4] = { 0 };
	ipBytes[0] = _packet.GetUInt8();
	ipBytes[1] = _packet.GetUInt8();
	ipBytes[2] = _packet.GetUInt8();
	ipBytes[3] = _packet.GetUInt8();

	PacketType type;
	Packet p;
	// db에서 조회 후 없으면 db에 기록 후 User 생성
	// 조회 후 있으면 db에서 가져온 후 User 생성
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	SQLLEN len = SQL_NTSL;
	uint32 userCnt = 0;

	DBConnection* pConn = DBConnectionManager::GetInst()->Acquire();
	pConn->Unbind();
	
	wchar_t query[256];
	swprintf(query, sizeof(query) / sizeof(wchar_t), L"SELECT COUNT(*) FROM Users WHERE nickname = '%ls';", pNickname);
	assert(pConn->BindCol(1, SQL_C_LONG, sizeof(userCnt), &userCnt, &len));
	assert(pConn->Execute(query));
	assert(pConn->Fetch());
	pConn->Unbind();

	len = SQL_NTSL;
	uint32 loginCnt = 1;
	User* pUser = nullptr;
	uint32 connID = _conn.GetId();
	uint32 dbConnID = USER_NOT_CONNECTED;

	if (userCnt == 0) // 아직 DB에 없음
	{
		SQLULEN nicknameLen = static_cast<SQLULEN>((::wcslen(pNickname) + 1) * 2);
		assert(pConn->BindParam(1, SQL_C_WCHAR, SQL_WVARCHAR, nicknameLen, (SQLPOINTER)pNickname, &len));
		assert(pConn->BindParam(2, SQL_C_LONG, SQL_INTEGER, sizeof(connID), &connID, &len));

		if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
		{
			type = static_cast<PacketType>(eServer::LoginFailure_Full);
			p.Add<PacketType>(type);
		}
		else
		{
			type = static_cast<PacketType>(eServer::LoginSuccess);
			p.Add<PacketType>(type);

			_conn.SetPrivateIP(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
			pUser = UserManager::GetInst()->Create(connID, pNickname);
			pLobby->Enter(_conn, pUser);
			
			assert(pConn->Execute(L"INSERT INTO Users(nickname, connID) VALUES(?, ?);"));
			wprintf(L"nickname : %s, loginCnt : %d\n", pNickname, loginCnt);
		}
	}
	else
	{
		swprintf(query, sizeof(query) / sizeof(wchar_t), L"SELECT id, loginCnt, connID from Users where nickname = '%ls';", pNickname);
		uint32 id = 0;
		assert(pConn->BindCol(1, SQL_C_LONG, sizeof(id), &id, &len));
		assert(pConn->BindCol(2, SQL_C_LONG, sizeof(loginCnt), &loginCnt, &len));
		assert(pConn->BindCol(3, SQL_C_LONG, sizeof(dbConnID), &dbConnID, &len));
		assert(pConn->Execute(query));
		assert(pConn->Fetch());
		pConn->Unbind();
		len = SQL_NTSL;

		if (dbConnID != USER_NOT_CONNECTED)
		{
			type = static_cast<PacketType>(eServer::LoginFailure_AlreadyLoggedIn);
			p.Add<PacketType>(type);
		}
		else
		{
			if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
			{
				type = static_cast<PacketType>(eServer::LoginFailure_Full);
				p.Add<PacketType>(type);
			}
			else
			{
				type = static_cast<PacketType>(eServer::LoginSuccess);
				p.Add<PacketType>(type);

				_conn.SetPrivateIP(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
				pUser = UserManager::GetInst()->Create(_conn.GetId(), pNickname);
				pLobby->Enter(_conn, pUser);

				swprintf(query, sizeof(query) / sizeof(wchar_t), L"UPDATE Users SET loginCnt = ?, connID = ? where id = %d;", id);
				++loginCnt;
				assert(pConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(loginCnt), &loginCnt, &len));
				assert(pConn->BindParam(2, SQL_C_LONG, SQL_INTEGER, sizeof(connID), &connID, &len));
				assert(pConn->Execute(query));

				wprintf(L"id : %d, nickname : %s, loginCnt : %d\n", id, pNickname, loginCnt);
			}
		}
	}

	DBConnectionManager::GetInst()->Release();

	wprintf(L"%s님 [%d] : ", pNickname, static_cast<int32>(_conn.GetSocket()));
	if (static_cast<eServer>(type) == eServer::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	_conn.Send(p);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "로그인 소요 시간 : " << duration.count() << "ms\n";
}