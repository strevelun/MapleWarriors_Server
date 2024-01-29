#include "LoginPacketHandler.h"
#include "../../User/UserManager.h"
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
	Packet p;

	if (pUser->GetState() == eLoginState::Logout)
	{
		if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
		{
			type = (PacketType)eServer::LoginFailure_Full;
			p.Add<PacketType>(type);
		}
		else
		{
			type = (PacketType)eServer::LoginSuccess;
			p.Add<PacketType>(type);
			p.Add<char>(_conn.GetId());
			UserManager::GetInst()->Connect(_conn.GetId(), pUser);
			pLobby->Enter(_conn, pUser);
		}
	}
	else // �ش���̵�� ������ �α���
	{
		type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
		p.Add<PacketType>(type);
	}

	wprintf(L"%s�� [%d] : ", pNickname, (int)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "�α��� ����!" << '\n';
	else std::cout << "�α��� ����!" << '\n';

	_conn.Send(p);
}