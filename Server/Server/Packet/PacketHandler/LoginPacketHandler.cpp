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
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	PacketType type;

	if (pUser->GetState() == eLoginState::Logout)
	{
		if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
			type = (PacketType)eServer::LoginFailure_Full;
		else
		{
			type = (PacketType)eServer::LoginSuccess;
			pUser->SetState(eLoginState::Login);
			_conn.SetUser(pUser);
			pLobby->Enter(_conn);
		}
	}
	else // 해당아이디로 누군가 로그인
	{
		type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
	}

	wprintf(L"%s님 [%d] : ", pNickname, (int)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	Packet p;
	p.Add<PacketType>(type);
	_conn.Send(p);
}