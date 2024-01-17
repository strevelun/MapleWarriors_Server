#include "LoginPacketHandler.h"
#include "../../UserManager.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"


void Login::Test(Connection& _conn, PacketReader& _packet)
{
}

void Login::Exit(Connection& _conn, PacketReader& _packet)
{
	_conn.SetDeleted();
}

void Login::LoginReq(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pNickname = _packet.GetWString();
	User* pUser = UserManager::GetInst()->Create(pNickname);
	if (pUser->GetState() == eLoginState::Logout)
	{
		pUser->SetState(eLoginState::Login);
		_conn.SetUser(pUser);
	}

	PacketType type = PacketType(_conn.GetUser() ? eServer::LoginSuccess : eServer::LoginFailure_AlreadyLoggedIn);

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (_conn.GetUser())
	{
		type = (PacketType)eServer::LoginSuccess;
		pLobby->Enter(_conn);
	}
	else
	{
		if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
			type = (PacketType)eServer::LoginFailure_Full;
		else
			type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
	}

	wprintf(L"%s님 [%d] : ", pNickname, (int)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	Packet p;
	p.Add<PacketType>(type);
	_conn.Send(p);
}